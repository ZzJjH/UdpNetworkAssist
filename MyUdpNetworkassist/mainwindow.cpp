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

/*********************************************************************************************/

/*发送数据*/
void MainWindow::on_SendBtn_clicked()
{
    int Aimport = ui->UdpPORTlineEdit->text().toInt();
    QString Aimip = ui->UdpIPlineEdit->text();
    QString sendText = ui->SendtextEdit->toPlainText();

    msocket->writeDatagram(sendText.toUtf8(),QHostAddress(Aimip),Aimport);
}

/**************************************************************************************/

/*接收信息槽函数*/
//Receive from[IP,端口号](时间)：
//接收内容
void MainWindow::readyRead_Slot()
{
    //接收发送者的IP和端口号
    QHostAddress AimIP;
    quint16 AimPort;


    while(msocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(msocket->pendingDatagramSize());

        //读取接收信息，IP和端口号
        msocket->readDatagram(datagram.data(),datagram.size(),&AimIP,&AimPort);

        //转化为可显示文本
        QString buf;
        buf = QString::fromUtf8(datagram); // 使用正确的编码方式

        //获取当前时间并转化为固定格式 (年-月-日 小时:分钟:秒)
        QDateTime currentTime = QDateTime::currentDateTime();
        QString Time = currentTime.toString("yyyy-MM-dd HH:mm:ss");

        //qDebug()<<"666"<<AimIP.toString()<<"666"<<QString::number(AimPort, 10);
        //666 "::ffff:192.168.137.1" 666 "56050"
        // 找到最后一个冒号的位置
        int lastColonIndex = AimIP.toString().lastIndexOf(':');// 提取冒号后面的部分
        QString ipAddress = AimIP.toString().mid(lastColonIndex + 1);

       //拼接显示的文本
        QString rece = QString("Receive from[%1,%2](%3)：\n%4")
                           .arg(ipAddress)
                           .arg(QString::number(AimPort, 10))
                           .arg(Time)
                           .arg(buf);

        ui->ReceiveplainTextEdit->appendPlainText(rece);

    }
}

/**********************************************************************************/


void MainWindow::on_saveBtn_clicked()
{
    QString currentPath = QDir::currentPath();
    QString title = "另存为一个文件";

    QString filename = QFileDialog::getSaveFileName(this,tr("将接收的文本数据另存为"),currentPath,tr("*.xlsx"));
//        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
//                                       "/home/jana/untitled.png",
//                                       tr("Images (*.png *.xpm *.jpg)"));
    if(filename.isEmpty())
    {
        return;
        //QMessageBox::warning(this,"警告","请选择一个文件");//点取消弹出警告
    }
    else
    {
        QFile file(filename);
        file.open(QIODevice::WriteOnly);
        QString receData = ui->ReceiveplainTextEdit->toPlainText();
        //QByteArray receData1 = receData.toLatin1();//中文乱码
        QByteArray receData1 = receData.toUtf8();
        file.write(receData1);
        QMessageBox::information(this,"提示","保存成功！");
        file.close();
    }
}

