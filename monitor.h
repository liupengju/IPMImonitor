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
    ~Monitor();



private:
    Ui::Monitor *ui;
    loginDialog *logDlg;

};

#endif // MONITOR_H
