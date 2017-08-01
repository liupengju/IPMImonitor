#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QMainWindow>
#include "newcntdialog.h"
#include "global.h"
#include <QDebug>
#include <QTableWidgetItem>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QVector>
#include <QMap>


namespace Ui {
class loginDialog;
}

class loginDialog : public QMainWindow
{
    Q_OBJECT

public:
    explicit loginDialog(QWidget *parent = 0);
    int getExistHost();
    int addHostToFile();
    int initLoginDlg();
    ~loginDialog();

private:
    Ui::loginDialog *ui;
    newCntDialog *cntDlg;
    QVector< QMap<QString,QString> > mHostList;
    void setupUI();

private slots:
    void newConnectter();
    void addHost();
    void on_closeBtn_clicked();
    void on_hostListWidget_clicked(const QModelIndex &index);
    int refreshUI();
};

#endif // LOGINDIALOG_H
