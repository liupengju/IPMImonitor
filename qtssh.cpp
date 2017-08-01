#include "qtssh.h"

QtSsh::QtSsh()
{

}

QtSsh::QtSsh(const char *host, int port)
{
    mHost = host;
    mPort = port;
}

int QtSsh::setSocketVersion()
{
    WSADATA wsadata;
    int err = WSAStartup(MAKEWORD(2,0), &wsadata);
    if (err != 0)
    {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
    }
}

int QtSsh::connectSSH()
{
    unsigned long hostaddr;
    struct sockaddr_in sin;
    int rc;

    rc = libssh2_init (0);
    if (rc != 0) {
        fprintf (stderr, "libssh2 initialization failed (%d)\n", rc);
        return 1;
    }
    hostaddr = inet_addr(mHost);
    mSock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(mPort);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(mSock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
    }
    return 0;
}

int QtSsh::sessionInit()
{
    int rc = 0;
    mSession = libssh2_session_init();

    if (!mSession)
        return -1;

    /* tell libssh2 we want it all done non-blocking */
    libssh2_session_set_blocking(mSession, 0);


    /* ... start it up. This will trade welcome banners, exchange keys,
     * and setup crypto, compression, and MAC layers
     */
    while ((rc = libssh2_session_handshake(mSession, mSock)) == LIBSSH2_ERROR_EAGAIN);
    if (rc)
    {
        fprintf(stderr, "Failure establishing SSH session: %d\n", rc);
        return -1;
    }
}

int QtSsh::authenticateHost()
{
    const char *fingerprint;
    int rc;
    LIBSSH2_KNOWNHOSTS *nh;
    size_t len;
    int type;

    nh = libssh2_knownhost_init(mSession);
    if(!nh)
    {
        /* eeek, do cleanup here */
        return 2;
    }

    /* read all hosts from here */
    libssh2_knownhost_readfile(nh, "known_hosts",LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    /* store all known hosts to here */
    libssh2_knownhost_writefile(nh, "dumpfile",LIBSSH2_KNOWNHOST_FILE_OPENSSH);

    fingerprint = libssh2_session_hostkey(mSession, &len, &type);

    if(fingerprint)
    {
        struct libssh2_knownhost *host;
        int check = libssh2_knownhost_checkp(nh, mHost, mPort,
                                             fingerprint, len,
                                             LIBSSH2_KNOWNHOST_TYPE_PLAIN|
                                             LIBSSH2_KNOWNHOST_KEYENC_RAW,
                                             &host);

        fprintf(stderr, "Host check: %d, key: %s\n", check,
                (check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)?
                    host->key:"<none>");

        /*****
         * At this point, we could verify that 'check' tells us the key is
         * fine or bail out.
         *****/
    }
    else
    {
        /* eeek, do cleanup here */
        return 3;
    }
    libssh2_knownhost_free(nh);

    if ( strlen(mPassword) != 0 )
    {
        /* We could authenticate via password */
        while ((rc = libssh2_userauth_password(mSession, mUsername, mPassword)) ==

               LIBSSH2_ERROR_EAGAIN);
        if (rc) {
            fprintf(stderr, "Authentication by password failed.\n");
            clearShhSource();
        }
    }
    else
    {
        /* Or by public key */
        while ((rc = libssh2_userauth_publickey_fromfile(mSession, mUsername,

                                                         "/home/user/"
                                                         ".ssh/id_rsa.pub",
                                                         "/home/user/"
                                                         ".ssh/id_rsa",
                                                         mPassword)) ==
               LIBSSH2_ERROR_EAGAIN);
        if (rc)
        {
            fprintf(stderr, "\tAuthentication by public key failed\n");
            clearShhSource();
        }
    }
}


int QtSsh::clearShhSource()
{
    libssh2_session_disconnect(mSession,"Normal Shutdown, Thank you for playing");
    libssh2_session_free(mSession);
    closesocket(mSock);
    fprintf(stderr, "all done\n");
    libssh2_exit();
}

int QtSsh::execCmd(const char *commandline)
{
    LIBSSH2_CHANNEL *channel;
    int rc;
    mExitsignal=(char *)"none";
    int bytecount = 0;
    mReceived.clear();
    mErrReceive.clear();

    /* Exec non-blocking on the remove host */
    while( (channel = libssh2_channel_open_session(mSession)) == NULL &&
            libssh2_session_last_error(mSession,NULL,NULL,0) ==LIBSSH2_ERROR_EAGAIN )
    {
        waitSocket();
    }
    if( channel == NULL )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }
    while( (rc = libssh2_channel_exec(channel, commandline)) == LIBSSH2_ERROR_EAGAIN )
    {
        waitSocket();
    }
    if( rc != 0 )
    {
        fprintf(stderr,"Error\n");
        exit( 1 );
    }
    for( ;; )
    {
        /* loop until we block */
        int rc;
        do
        {
            char buffer[0x4000];
            char errBuffer[0x4000];
            rc = libssh2_channel_read( channel, buffer, sizeof(buffer) );
            int errRc =  libssh2_channel_read_stderr( channel, errBuffer, sizeof(errBuffer));
            if( rc > 0 )
            {
                bytecount += rc;
                mReceived.append(buffer);
            }
            else
            {
                if( rc != LIBSSH2_ERROR_EAGAIN )
                    fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
            }
            if( errRc > 0 )
            {
                mErrReceive.append(errBuffer);
            }
            else
            {
                if( rc != LIBSSH2_ERROR_EAGAIN )
                    fprintf(stderr, "libssh2_channel_read returned %d\n", rc);
            }
        }
        while( rc > 0 );

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if( rc == LIBSSH2_ERROR_EAGAIN )
        {
            waitSocket();
        }
        else
            break;
    }
    mExitcode = 127;
    while( (rc = libssh2_channel_close(channel)) == LIBSSH2_ERROR_EAGAIN )
        waitSocket();

    if( rc == 0 )
    {
        mExitcode = libssh2_channel_get_exit_status( channel );
        libssh2_channel_get_exit_signal(channel, &mExitsignal,NULL, NULL, NULL, NULL, NULL);
    }
/*
    if (mExitsignal)
        fprintf(stderr, "\nGot signal: %s\n", mExitsignal);
    else
        fprintf(stderr, "\nEXIT: %d bytecount: %d\n", mExitcode, bytecount);
*/
    libssh2_channel_free(channel);
    channel = NULL;
    return 0;
}

int QtSsh::waitSocket()
{
    struct timeval timeout;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    FD_ZERO(&fd);
    FD_SET(mSock, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(mSession);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;
    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;
    rc = select(mSock + 1, readfd, writefd, NULL, &timeout);
    return rc;
}

int QtSsh::setUsername(const char* name)
{
    mUsername = name;
    return 0;
}

int QtSsh::setPasswd(const char *passwd)
{
    mPassword = passwd;
    return 0;
}
