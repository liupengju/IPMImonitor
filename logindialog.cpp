#include "logindialog.h"
#include "ui_logindialog.h"

loginDialog::loginDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::loginDialog)
{
    ui->setupUi(this);
    this->setupUI();
    initLoginDlg();
}

loginDialog::~loginDialog()
{
    delete ui;
}

void loginDialog::setupUI()
{
    QAction *propertyAction = new QAction(QIcon(":/icon/active/property.png"),"属性");
    QAction *uaPropertyAction = new QAction(QIcon(":/icon/unactive/property.png"),"属性");
    QAction *newAction = new QAction(QIcon(":/icon/active/new.png"),"新建");
    QAction *uaNewAction = new QAction(QIcon(":/icon/unactive/new.png"),"新建");
    connect(newAction,SIGNAL(triggered(bool)),this,SLOT(newConnectter()));
    QAction *copyAction = new QAction(QIcon(":/icon/active/copy.png"),"复制");
    QAction *uaCopyAction = new QAction(QIcon(":/icon/unactive/copy.png"),"复制");
    QAction *delAction = new QAction(QIcon(":/icon/active/del.png"),"删除");
    QAction *uaDelAction = new QAction(QIcon(":/icon/unactive/del.png"),"删除");
    QAction *refreshAction = new QAction(QIcon(":/icon/active/refresh.png"),"刷新");
    connect(refreshAction,SIGNAL(triggered(bool)),this,SLOT(refreshUI()));
    QAction *uaRefreshAction = new QAction(QIcon(":/icon/unactive/refresh.png"),"刷新");
    QAction *backAction = new QAction(QIcon(":/icon/active/back.png"),"返回");
    QAction *uaBackAction = new QAction(QIcon(":/icon/unactive/back.png"),"返回");
    ui->toolBar->addAction(propertyAction);
    ui->toolBar->addAction(uaPropertyAction);
    ui->toolBar->addAction(newAction);
    ui->toolBar->addAction(uaNewAction);
    ui->toolBar->addAction(copyAction);
    ui->toolBar->addAction(uaCopyAction);
    ui->toolBar->addAction(delAction);
    ui->toolBar->addAction(uaDelAction);
    ui->toolBar->addAction(refreshAction);
    ui->toolBar->addAction(uaRefreshAction);
    ui->toolBar->addAction(backAction);
    ui->toolBar->addAction(uaBackAction);
}

int loginDialog::initLoginDlg()
{
    //讲xml文件写入到mHostList中
    getExistHost();
    //添加host列表到QTableWidget中
    for(int i =0;i<mHostList.size();i++)
    {
        ui->hostListWidget->insertRow(i);
        ui->hostListWidget->setItem(i,0,new QTableWidgetItem(mHostList[i]["name"]));
        ui->hostListWidget->setItem(i,1,new QTableWidgetItem(mHostList[i]["host"]));
        ui->hostListWidget->setItem(i,2,new QTableWidgetItem(mHostList[i]["port"]));
        ui->hostListWidget->setItem(i,3,new QTableWidgetItem(mHostList[i]["prot"]));
        ui->hostListWidget->setItem(i,4,new QTableWidgetItem(mHostList[i]["user"]));
    }

}

int loginDialog::refreshUI()
{
    //讲xml文件写入到mHostList中
    getExistHost();
    int rowCount = ui->hostListWidget->rowCount();
    while(rowCount--)
    {
        qDebug()<<rowCount<<ui->hostListWidget->rowCount();
        ui->hostListWidget->removeRow(rowCount);
    }

    //添加host列表到QTableWidget中
    for(int i =0;i<mHostList.size();i++)
    {
        ui->hostListWidget->insertRow(i);
        ui->hostListWidget->setItem(i,0,new QTableWidgetItem(mHostList[i]["name"]));
        ui->hostListWidget->setItem(i,1,new QTableWidgetItem(mHostList[i]["host"]));
        ui->hostListWidget->setItem(i,2,new QTableWidgetItem(mHostList[i]["port"]));
        ui->hostListWidget->setItem(i,3,new QTableWidgetItem(mHostList[i]["prot"]));
        ui->hostListWidget->setItem(i,4,new QTableWidgetItem(mHostList[i]["user"]));
    }

}

void loginDialog::newConnectter()
{
    qDebug()<<"newConnectter";
    cntDlg = new newCntDialog(this);
    connect(cntDlg,SIGNAL(hostAdded()),this,SLOT(addHost()));
    cntDlg->show();
}

void loginDialog::on_closeBtn_clicked()
{
    this->close();
}

void loginDialog::addHost()
{
    //将新增的host信息添加到文件中
    addHostToFile();
    //将新增的host信息添加到TableWidget中
    int currentRow = ui->hostListWidget->rowCount();
    ui->hostListWidget->insertRow(currentRow);
    ui->hostListWidget->setItem(currentRow,0,new QTableWidgetItem(cntDlg->getName()));
    ui->hostListWidget->setItem(currentRow,1,new QTableWidgetItem(cntDlg->getHost()));
    ui->hostListWidget->setItem(currentRow,2,new QTableWidgetItem(cntDlg->getPort()));
    ui->hostListWidget->setItem(currentRow,3,new QTableWidgetItem(cntDlg->getProtol()));
    ui->hostListWidget->setItem(currentRow,4,new QTableWidgetItem(cntDlg->getUser()));
}

/**
 * @brief loginDialog::addHostToFile
 * @
 * @      因为xmlStreamWriter写入文的时候采用的是覆盖重写，因此，需要将源
 * @      xml文件读取到内存中，然后将内存和新增的信息一起写入到xml文件中
 */
int loginDialog::addHostToFile()
{
   QXmlStreamWriter xmlWriter;
   //1,将已存在的host信息从xml文件中读取到内存中
   getExistHost();
   QFile file(HOSTLISTXML);
   if(!file.open(QFile::WriteOnly|QFile::Text))
   {
       qDebug()<<"cat not open file to write "<<HOSTLISTXML;
       qDebug()<<file.errorString();
       return -1;
   }
   xmlWriter.setDevice(&file);
   xmlWriter.setAutoFormatting(true);
   xmlWriter.writeStartDocument();
   xmlWriter.writeStartElement("Root");
   //2,将mHostList写入到文件中
   for(int i = 0; i<mHostList.size();i++)
   {
       xmlWriter.writeStartElement("Host");
       xmlWriter.writeAttribute("name",mHostList[i]["name"]);
       xmlWriter.writeAttribute("host",mHostList[i]["host"]);
       xmlWriter.writeAttribute("port",mHostList[i]["port"]);
       xmlWriter.writeAttribute("protol",mHostList[i]["prot"]);
       xmlWriter.writeAttribute("user",mHostList[i]["user"]);
       xmlWriter.writeAttribute("passwd",mHostList[i]["pswd"]);
       xmlWriter.writeEndElement();
   }
   //3,将新增的host信息写入到xml文件末尾
   xmlWriter.writeStartElement("Host");
   xmlWriter.writeAttribute("name",cntDlg->getName());
   xmlWriter.writeAttribute("host",cntDlg->getHost());
   xmlWriter.writeAttribute("port",cntDlg->getPort());
   xmlWriter.writeAttribute("protol",cntDlg->getProtol());
   xmlWriter.writeAttribute("user",cntDlg->getUser());
   xmlWriter.writeAttribute("passwd",cntDlg->getpswd());
   xmlWriter.writeEndElement();
   xmlWriter.writeEndDocument();
   xmlWriter.writeEndDocument();
   file.close();
   return 0;
}


int loginDialog::getExistHost()
{
    QMap<QString,QString>hostItem;
    mHostList.clear();
    QXmlStreamReader xmlReader;
    QFile file(HOSTLISTXML);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        qDebug()<<"can not open file to read "<<HOSTLISTXML;
        return -1;
    }
    xmlReader.setDevice(&file);
    int i = 0;
    while(!xmlReader.atEnd())
    {
        if(xmlReader.isStartElement())
        {
            if(xmlReader.name() != "Host")
            {
                qDebug()<<xmlReader.name();
                xmlReader.readNext();
                continue;
            }
            //解析xml文件中的host节点属性到MAP中
            QXmlStreamAttributes attributes = xmlReader.attributes();
            if(attributes.hasAttribute("name"))
            {
                hostItem["name"] = attributes.value("name").toString();
            }
             if(attributes.hasAttribute("host"))
            {
                hostItem["host"] = attributes.value("host").toString();
            }
             if(attributes.hasAttribute("port"))
            {
                hostItem["port"] = attributes.value("port").toString();
            }
             if(attributes.hasAttribute("protol"))
            {
                hostItem["prot"] = attributes.value("protol").toString();
            }
             if(attributes.hasAttribute("user"))
            {
                hostItem["user"] = attributes.value("user").toString();
            }
             if(attributes.hasAttribute("passwd"))
            {
                hostItem["pswd"] = attributes.value("passwd").toString();
            }
            mHostList.append(hostItem);
        }
        xmlReader.readNext();
    }
    file.close();
    xmlReader.clear();
}

void loginDialog::on_connectBtn_clicked()
{
   //获取当前选中host信息
    QMap<QString,QString> hostItem;
    const QList <QTableWidgetItem *> list = ui->hostListWidget->selectedItems();
    if(list.isEmpty())
    {
        return ;
    }
    hostItem["name"] = list[0]->text();
    hostItem["host"] = list[1]->text();
    hostItem["port"] = list[2]->text();
    hostItem["prto"] = list[3]->text();
    hostItem["user"] = list[4]->text();
    for(int i = 0; i<mHostList.size();i++)
    {
        if(mHostList[i]["name"] == hostItem["name"])
        {
            hostItem["pswd"] = mHostList[i]["pswd"];
        }
    }
    qDebug()<<"emit signal readyConnectHost";
    emit readyConnectHost(hostItem);
}

void loginDialog::on_hostListWidget_doubleClicked(const QModelIndex &index)
{
    QMap<QString,QString> hostItem;

    int row = index.row();
    QString name = ui->hostListWidget->item(row,0)->text();
    QString host = ui->hostListWidget->item(row,1)->text();
    QString port = ui->hostListWidget->item(row,2)->text();
    QString prot = ui->hostListWidget->item(row,3)->text();
    QString user = ui->hostListWidget->item(row,4)->text();

    hostItem["name"] = name;
    hostItem["host"] = host;
    hostItem["port"] = port;
    hostItem["prto"] = prot;
    hostItem["user"] = user;
    for(int i = 0; i<mHostList.size();i++)
    {
        if(mHostList[i]["name"] == hostItem["name"])
        {
            hostItem["pswd"] = mHostList[i]["pswd"];
        }
    }

    qDebug()<<host<<name<<port<<prot<<user<<hostItem["pswd"];
    emit readyConnectHost(hostItem);
}
