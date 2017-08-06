#include "monitor.h"
#include "ui_monitor.h"
#include <QPushButton>

Monitor::Monitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Monitor)
{
    ui->setupUi(this);
    connect(ui->showCmdEditAct,SIGNAL(triggered(bool)),this,SLOT(showCmdTextEdit()));
    setWindowLayout();
}

Monitor::~Monitor()
{
    delete ui;
}

int Monitor::showCmdTextEdit()
{
    if(ui->showCmdEditAct->isChecked())
    {
        ui->cmdTextEdit->show();
    }
    else
    {
        ui->cmdTextEdit->hide();
    }
    return 0;
}

int Monitor::setWindowLayout()
{
    loginDialog *logDlg = new loginDialog(this);
    connect(ui->openLoginDlgAct,SIGNAL(triggered(bool)),logDlg,SLOT(show()));
    connect(logDlg,SIGNAL(readyConnectHost(QMap<QString,QString>)),this,SLOT(newConnecter(QMap<QString,QString>)));
    ui->cmdTextEdit->show();
    logDlg->show();
    return 0;
}

int Monitor::newConnecter(QMap<QString, QString> hostItem)
{
    qDebug()<<hostItem["name"]<<hostItem["host"]<<hostItem["port"]<<hostItem["prto"]<<hostItem["user"]<<hostItem["pswd"];
    QtSsh *ssh = new QtSsh(hostItem["host"].toStdString().data(),hostItem["port"].toInt());
    ssh->setUsername(hostItem["user"].toStdString().data());
    ssh->setPasswd(hostItem["pswd"].toStdString().data());
    ssh->setSocketVersion();
    if(ssh->connectSSH())
    {
        QMessageBox::critical(this,"Error",QString("Connect to %1 Error\n%2").arg(hostItem["host"]).arg(ssh->mLastError),
                QMessageBox::Yes|QMessageBox::Yes);
        return 1;
    }

    if(ssh->sessionInit())
    {
        QMessageBox::critical(this,"Error",QString("SessionInit for %1 Error\n%2").arg(hostItem["host"]).arg(ssh->mLastError),
                QMessageBox::Yes|QMessageBox::Yes);
        return 1;
    }

    if(ssh->authenticateHost())
    {
        QMessageBox::critical(this,"Error",QString("Authenticate to %1 Error\n%2").arg(hostItem["host"]).arg(ssh->mLastError),
                QMessageBox::Yes|QMessageBox::Yes);
        return 1;
    }

    if(ssh->execCmd("ifconfig"))
    {
        QMessageBox::critical(this,"Error",QString("ExitCmd for  %1 Error\n%2").arg(hostItem["host"]).arg(ssh->mLastError),
                QMessageBox::Yes|QMessageBox::Yes);
        return 1;
    }
    if(!ssh->mReceived.isEmpty())
    {
        ui->cmdTextEdit->set
        ui->cmdTextEdit->appendPlainText();
    }
    return 0;
}
