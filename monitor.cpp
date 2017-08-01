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
    ui->cmdTextEdit->show();
    return 0;
}
