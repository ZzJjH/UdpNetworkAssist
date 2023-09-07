#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>

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

