#include "logindialog.h"
#include "ui_logindialog.h"

loginDialog::loginDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    this->setupUI();
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::setupUI()
{
    QAction *propertyAction = new QAction(QIcon(":/icon/active/property.png"),"属性");
    QAction *uaPropertyAction = new QAction(QIcon(":/icon/unactive/property.png"),"属性");
    QAction *newAction = new QAction(QIcon(":/icon/active/new.png"),"新建");
    QAction *uaNewAction = new QAction(QIcon(":/icon/unactive/new.png"),"新建");
    connect(newAction,SIGNAL(triggered(bool)),this,SLOT(newConnectter()));
    QAction *copyAction = new QAction(QIcon(":/icon/active/copy.png"),"复制");
    QAction *uaCopyAction = new QAction(QIcon(":/icon/unactive/copy.png"),"复制");
    QAction *delAction = new QAction(QIcon(":/icon/active/del.png"),"删除");
    QAction *uaDelAction = new QAction(QIcon(":/icon/unactive/del.png"),"删除");
    QAction *refreshAction = new QAction(QIcon(":/icon/active/refresh.png"),"刷新");
    QAction *uaRefreshAction = new QAction(QIcon(":/icon/unactive/refresh.png"),"刷新");
    QAction *backAction = new QAction(QIcon(":/icon/active/back.png"),"返回");
    QAction *uaBackAction = new QAction(QIcon(":/icon/unactive/back.png"),"返回");
    ui->toolBar->addAction(propertyAction);
    ui->toolBar->addAction(uaPropertyAction);
    ui->toolBar->addAction(newAction);
    ui->toolBar->addAction(uaNewAction);
    ui->toolBar->addAction(copyAction);
    ui->toolBar->addAction(uaCopyAction);
    ui->toolBar->addAction(delAction);
    ui->toolBar->addAction(uaDelAction);
    ui->toolBar->addAction(refreshAction);
    ui->toolBar->addAction(uaRefreshAction);
    ui->toolBar->addAction(backAction);
    ui->toolBar->addAction(uaBackAction);
}
void loginDialog::newConnectter()
{
    qDebug()<<"newConnectter";
    newCntDialog *cntDlg = new newCntDialog(this);
    cntDlg->show();
}

void loginDialog::on_closeBtn_clicked()
{
    this->close();
}
