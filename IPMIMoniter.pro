#-------------------------------------------------
#
# Project created by QtCreator 2017-07-07T23:33:57
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = IPMIMoniter
TEMPLATE = app

LIBS+= -LD:/0pro_Software/MinGW/3rdLib/libssh2/libcurl/lib/  -lssh2 -lcurl -lws2_32 -lwldap32 -lssl -lcrypto -lz
INCLUDEPATH += D:/0pro_Software/MinGW/3rdLib/libssh2/libcurl/include
INCLUDEPATH += D:/0pro_Software/MinGW/3rdLib/libssh2/libcurl/include/openssl
INCLUDEPATH += D:/0pro_Software/MinGW/3rdLib/libssh2/libcurl/include/curl


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        monitor.cpp \
    logindialog.cpp \
    newcntdialog.cpp \
    global.cpp \
    qtssh.cpp

HEADERS  += monitor.h \
    logindialog.h \
    newcntdialog.h \
    global.h \
    qtssh.h

FORMS    += monitor.ui \
    logindialog.ui \
    newcntdialog.ui

RESOURCES += \
    Res/res.qrc
