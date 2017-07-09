#include "monitor.h"
#include "ui_monitor.h"
#include <QPushButton>

Monitor::Monitor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Monitor)
{
    ui->setupUi(this);

    loginDialog *logDlg = new loginDialog(this);
    logDlg->show();
}

Monitor::~Monitor()
{
    delete ui;
}
