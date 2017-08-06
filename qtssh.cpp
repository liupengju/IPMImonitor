#include "qtssh.h"

QtSsh::QtSsh()
{

}

QtSsh::QtSsh(const char *host, int port)
{
    mHost = host;
    mPort = port;
    mLastError = "";
}

int QtSsh::setSocketVersion()
{
    WSADATA wsadata;
    int err = WSAStartup(MAKEWORD(2,0), &wsadata);
    if (err != 0)
    {
        mLastError = QString("WSAStartup failed with error: %1").arg(err);
        return 1;
    }
}

int QtSsh::connectSSH()
{
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
    unsigned long hostaddr;
    struct sockaddr_in sin;
    int rc;

    rc = libssh2_init (0);
    if (rc != 0) {
        mLastError =  QString("libssh2 initialization failed (%1)").arg(rc);
        return 1;
    }
    hostaddr = inet_addr(mHost);
    mSock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(mPort);
    sin.sin_addr.s_addr = hostaddr;
    if (connect(mSock, (struct sockaddr*)(&sin),
                sizeof(struct sockaddr_in)) != 0) {
        mLastError =  QString("failed to connect!\n");
        return -1;
    }
    return 0;
}

int QtSsh::sessionInit()
{
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
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
        mLastError = QString("Failure establishing SSH session: %1").arg(rc);
        return -1;
    }
    return 0;
}

int QtSsh::authenticateHost()
{
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
    const char *fingerprint;
    int rc;
    LIBSSH2_KNOWNHOSTS *nh;
    size_t len;
    int type;

    nh = libssh2_knownhost_init(mSession);
    if(!nh)
    {
        mLastError = QString("libssh2_knownhost_init error");
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

        mLastError = QString("Host check: %1, key: %2\n").arg(check).arg((check <= LIBSSH2_KNOWNHOST_CHECK_MISMATCH)? host->key:"<none>");

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
        qDebug()<<mUsername<<mPassword;
        while ((rc = libssh2_userauth_password(mSession, mUsername, mPassword)) ==  LIBSSH2_ERROR_EAGAIN);
        if (rc)
        {
            mLastError = QString("Authentication by password failed.");
            clearShhSource();
            return 1;
        }
    }
    else
    {
        /* Or by public key */
        while ((rc = libssh2_userauth_publickey_fromfile(mSession, mUsername,
                                                         "/home/user/" ".ssh/id_rsa.pub",
                                                         "/home/user/" ".ssh/id_rsa",
                                                         mPassword)) == LIBSSH2_ERROR_EAGAIN);
        if (rc)
        {
            mLastError = QString("\tAuthentication by public key failed");
            clearShhSource();
            return -1;
        }
    }
    return 0;
}


int QtSsh::clearShhSource()
{
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
    libssh2_session_disconnect(mSession,"Normal Shutdown, Thank you for playing");
    libssh2_session_free(mSession);
    closesocket(mSock);
    fprintf(stderr, "all done\n");
    libssh2_exit();
}

int QtSsh::execCmd(const char *commandline)
{
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
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
        mLastError = QString("libssh2_channel_open_session Error");
        return( 1 );
    }
    while( (rc = libssh2_channel_exec(channel, commandline)) == LIBSSH2_ERROR_EAGAIN )
    {
        waitSocket();
    }
    if( rc != 0 )
    {
        QString("libssh2_channel_exec Error");
        return( 1 );
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
                    mLastError = QString("libssh2_channel_read returned %1\n").arg(rc);
            }
            if( errRc > 0 )
            {
                mErrReceive.append(errBuffer);
            }
            else
            {
                if( rc != LIBSSH2_ERROR_EAGAIN )
                    mLastError = QString("libssh2_channel_read returned %1").arg(rc);
            }
        }
        while( rc > 0 );

        /* this is due to blocking that would occur otherwise so we loop on
           this condition */
        if( rc == LIBSSH2_ERROR_EAGAIN )
            waitSocket();
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
    libssh2_channel_free(channel);
    channel = NULL;
    qDebug()<<"Enter Into "<<__FUNCTION__;
    return 0;
}

int QtSsh::waitSocket()
{
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
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
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
    mUsername = name;
    return 0;
}

int QtSsh::setPasswd(const char *passwd)
{
    qDebug()<<"Enter Into "<<__FUNCTION__<<__LINE__;
    mPassword = passwd;
    return 0;
}
