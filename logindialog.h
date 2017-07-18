#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QMainWindow>
#include "newcntdialog.h"
#include <QDebug>
#include <QTableWidgetItem>

namespace Ui {
class loginDialog;
}

class loginDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = 0);
    ~loginDialog();

private:
    Ui::loginDialog *ui;
    newCntDialog *cntDlg;
    void setupUI();

private slots:
    void newConnectter();
    void addHost();
    void on_closeBtn_clicked();
};

#endif // LOGINDIALOG_H
