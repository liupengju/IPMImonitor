#ifndef QTSSH_H
#define QTSSH_H
#include "libssh2_config.h"
#include <libssh2.h>
#ifdef HAVE_WINSOCK2_H
#include <winsock2.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#include <sys/types.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <QObject>
#include <qDebug>


/**
 * @brief QtSsh连接例子
    int main(int argc, char *argv[])
    {
        int i = 1;
        QtSsh ssh("192.168.0.131",22);
        ssh.setSocketVersion();
        ssh.connectSSH();
        ssh.sessionInit();
        ssh.authenticateHost();
        ssh.execCmd("ssh");
        qDebug()<<"==================================================\n"<<ssh.mReceived;
        qDebug()<<"======================err=========================\n"<<ssh.mErrReceive;
        ssh.clearShhSource();
        return 0;
    }
 */


class QtSsh
{
public:
    QtSsh();
    QtSsh(const char *host,int port);
    /**
     * @brief 为了在应用程序当中调用任何一个Winsock API函数，首先第一件事情就是必须通过WSAStartup函数完成对Winsock服务的初始化，
     * 因此需要调用WSAStartup函数。使用Socket的程序在使用Socket之前必须调用WSAStartup函数。该函数的第一个参数指明程序请求使用的
     * Socket版本，其中高位字节指明副版本、低位字节指明主版本；操作系统利用第二个参数返回请求的Socket的版本信息。当一个应用程序调用
     * WSAStartup函数时，操作系统根据请求的Socket版本来搜索相应的Socket库，然后绑定找到的Socket库到该应用程序中。以后应用程序就可
     * 以调用所请求的Socket库中的其它Socket函数了。
     */
    int setSocketVersion();
    int connectSSH();
    int sessionInit();
    int authenticateHost();
    int clearShhSource();
    int execCmd(const char * commandline);
    int waitSocket();
    int setUsername(const char* name);
    int setPasswd(const char* passwd);

public:
    const char *mHost;
    int mPort = 22;
    const char *mUsername;
    const char *mPassword;
    QString mLastError;
    //保存命令在远端执行成功的结果返回
    QString mReceived;
    //保存命令在远端执行失败的结果返回
    QString mErrReceive;
    int mExitcode;
    char *mExitsignal;

    int mSock;
    LIBSSH2_SESSION *mSession;
    LIBSSH2_CHANNEL *mChannel;
    //WSADATA mWsadata;
};

#endif // QTSSH_H
