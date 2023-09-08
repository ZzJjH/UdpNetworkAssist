#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>
#include<QDebug>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /*创建套接字对象*/
    msocket = new QUdpSocket(this);

    /*本地端口输入限制为数字*/
    PORTvalidator = new QIntValidator(this);
    //PORTvalidator->setRange(49152, 65535);
    ui->PortlineEdit->setValidator(PORTvalidator);

    /*获取本地IP地址：
       1、用QList存储本地所有IP地址
       2、用for循环遍历所有IP地址，并把本地IP地址显示在主界面。（条件：非 127.0.0.1 、非IPV6)
    */
    QList<QHostAddress> IPlist = QNetworkInterface::allAddresses();//#include<QtNetwork>
    for(int i = 0; i < IPlist.size(); ++i)
    {
        if(IPlist.at(i) != QHostAddress::LocalHost && IPlist.at(i).toIPv4Address()) {
            ui->IPlineEdit->setText(IPlist.at(i).toString());
            break;
        }
    }

    //接收信息1
//    connect(msocket,&QUdpSocket::readyRead,this,[this](){
//        char str[1024] = {0};
//        msocket->readDatagram(str,sizeof str);
//        ui->ReceivetextEdit->append(str);
//    });

    connect(msocket,&QUdpSocket::readyRead,this,&MainWindow::readyRead_Slot);

}

MainWindow::~MainWindow()
{
    delete ui;
}

//用户输入端口号后绑定
void MainWindow::on_BindBtn_clicked()
{
    int port = ui->PortlineEdit->text().toInt();
    bool flag = msocket->bind(port);
    if(flag)
    {
        //绑定成功
        ui->BindBtn->setEnabled(false);
        ui->PortlineEdit->setReadOnly(true);
       // ui->PortlineEdit->setEnabled(false);
        QMessageBox::information(this,"提示","绑定成功！");

    }
    else
    {
        //绑定失败
        QMessageBox::information(this,"提示","绑定失败！");
    }

}


void MainWindow::on_SendBtn_clicked()
{
    int Aimport = ui->UdpPORTlineEdit->text().toInt();
    QString Aimip = ui->UdpIPlineEdit->text();
    QString sendText = ui->SendtextEdit->toPlainText();

    msocket->writeDatagram(sendText.toUtf8(),QHostAddress(Aimip),Aimport);
}

void MainWindow::readyRead_Slot()
{
    QHostAddress AimIP;
    quint16 AimPort;
    while(msocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(msocket->pendingDatagramSize());

        msocket->readDatagram(datagram.data(),datagram.size(),&AimIP,&AimPort);

        QString buf;
        buf = QString::fromUtf8(datagram); // 使用正确的编码方式

        QDateTime currentTime = QDateTime::currentDateTime();
        QString Time = currentTime.toString("yyyy-MM-dd HH:mm:ss");
        //qDebug()<<"666"<<AimIP.toString()<<"666"<<QString::number(AimPort, 10);
        //666 "::ffff:192.168.137.1" 666 "56050"
        // 找到最后一个冒号的位置
        qDebug()<<"666"<<AimIP.toString()<<"666"<<QString::number(AimPort, 10);
        int lastColonIndex = AimIP.toString().lastIndexOf(':');// 提取冒号后面的部分
        QString ipAddress = AimIP.toString().mid(lastColonIndex + 1);

        QString rece = QString("Receive from[%1,%2](%3)：\n%4")
                           .arg(ipAddress)
                           .arg(QString::number(AimPort, 10))
                           .arg(Time)
                           .arg(buf);

        ui->ReceiveplainTextEdit->appendPlainText(rece);

    }
}

