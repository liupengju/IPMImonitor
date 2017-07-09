#ifndef NEWCNTDIALOG_H
#define NEWCNTDIALOG_H

#include <QDialog>
#include <QDebug>
#include <QMessageBox>
#include <QXmlStreamWriter>
#include <QFile>

namespace Ui {
class newCntDialog;
}

class newCntDialog : public QDialog
{
    Q_OBJECT

public:
    explicit newCntDialog(QWidget *parent = 0);
    ~newCntDialog();

private slots:
    void on_exitBtn_clicked();
    void on_ensureBtn_clicked();

private:
    Ui::newCntDialog *ui;
    QString mName;
    QString mHost;
    QString mProtol;
    QString port;
    QString mUserName;
    QString mPasswd;
};

#endif // NEWCNTDIALOG_H
