#ifndef MONITOR_H
#define MONITOR_H

#include <QMainWindow>
#include <logindialog.h>
#include "qtssh.h"
#include <QMap>
#include <QVector>
#include <QDebug>

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
    QVector<QtSsh*> sessionList;

private slots:
    int showCmdTextEdit();
    int newConnecter(QMap<QString,QString>);

};

#endif // MONITOR_H
