#include "newcntdialog.h"
#include "ui_newcntdialog.h"

newCntDialog::newCntDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::newCntDialog)
{
    qDebug()<<"newCntDialog Construct";
    ui->setupUi(this);
}

newCntDialog::~newCntDialog()
{
    delete ui;
    qDebug()<<"newCntDialog Destruct";
}

void newCntDialog::on_exitBtn_clicked()
{
    this->close();
}

void newCntDialog::on_ensureBtn_clicked()
{
    if(  ui->nameEdit->text().isEmpty()
         ||ui->hostEdit->text().isEmpty()
         ||ui->portEdit->text().isEmpty()
         ||ui->userEdit->text().isEmpty() )
    {
        QMessageBox::warning(this,"Warning","除密码外\n所有选项不能为空",QMessageBox::Ok|QMessageBox::No,QMessageBox::Ok);
    }
    //更新主机列表信息
    this->mName = ui->nameEdit->text();
    this->mHost = ui->hostEdit->text();
    this->mPort = ui->portEdit->text();
    this->mProtol = ui->protocolCom->currentText();
    this->mUserName = ui->userEdit->text();
    this->mPasswd = ui->passwdEdit->text();
    emit hostAdded();

    //this->close();
}
