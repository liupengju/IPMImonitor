#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QMainWindow>
#include "newcntdialog.h"
#include <QDebug>

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
    void setupUI();

private slots:
    void newConnectter();
    void on_closeBtn_clicked();
};

#endif // LOGINDIALOG_H
