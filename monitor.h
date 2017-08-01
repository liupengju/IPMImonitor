#ifndef MONITOR_H
#define MONITOR_H

#include <QMainWindow>
#include <logindialog.h>

namespace Ui {
class Monitor;
}

class Monitor : public QMainWindow
{
    Q_OBJECT

public:
    explicit Monitor(QWidget *parent = 0);
    int setWindowLayout();
    ~Monitor();



private:
    Ui::Monitor *ui;
    loginDialog *logDlg;

private slots:
    int showCmdTextEdit();

};

#endif // MONITOR_H
