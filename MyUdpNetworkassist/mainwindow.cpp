#include "mainwindow.h"
#include "ui_mainwindow.h"

#include<QMessageBox>
#include<QDebug>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initExcel();
    ui->cancel_bind_Btn->setEnabled(false);

    initCharts();

    /* 1. 获取本机ip并显示 2. 设置输入端口只能为数字 */
    default_ip_port();

    /* 创建套接字对象 */
    msocket = new QUdpSocket(this);

    /* Udp接收数据 */
    connect(msocket,&QUdpSocket::readyRead,this,&MainWindow::readyRead_Slot);

    /* 显示接收速率 */
    mtimer = new QTimer(this);
    mtimer->start(1000);
    connect(mtimer,&QTimer::timeout,this,&MainWindow::RecvRate_slot);
    lastUpdateTime = QDateTime::currentDateTime();
    lastTotalBytesReceived = totalBytesReceived;

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*********************************************************************************************/

/* 用户输入端口号后点击“绑定”按钮 */
void MainWindow::on_BindBtn_clicked()
{
    port_bind();
    accx_lineSeries->clear();
    pointNum = 0;
}

/*********************************************************************************************/

/* 发送数据 */
void MainWindow::on_SendBtn_clicked()
{
    send_message();
}

/**************************************************************************************/

/* 接收信息槽函数 */
// Receive from[IP,端口号](时间)：
// 接收内容
void MainWindow::readyRead_Slot()
{
    recv_message();
}

/**********************************************************************************/


void MainWindow::on_saveBtn_clicked()
{
    save_message(save_data);
}

void MainWindow::RecvRate_slot()
{
   QDateTime currentTime = QDateTime::currentDateTime();
   int timeElapsed = lastUpdateTime.msecsTo(currentTime); // 毫秒为单位
   qint64 bytesReceivedThisSecond = totalBytesReceived - lastTotalBytesReceived;
   receiveRate = (double)bytesReceivedThisSecond / (timeElapsed / 1000.0); // 字节每秒

   ui->recv_speed->setText(QString("接收速率: %1 字节/秒").arg(receiveRate));

 // 更新上次更新时间和总接收字节数
    lastUpdateTime = currentTime;
    lastTotalBytesReceived = totalBytesReceived;
}




void MainWindow::default_ip_port()
{
    /* 本地端口输入限制为数字 */
    PORTvalidator = new QIntValidator(this);
    ui->PortlineEdit->setValidator(PORTvalidator);

    /* 获取本地IP地址并显示：
        1、用QList存储本地所有IP地址
        2、用for循环遍历所有IP地址，并把本地IP地址显示在主界面。（条件：非 127.0.0.1 、非IPV6)
    */
    QList<QHostAddress> IPlist = QNetworkInterface::allAddresses();//#include<QtNetwork>
    for(int i = 0; i < IPlist.size(); ++i)
    {
        if(IPlist.at(i) != QHostAddress::LocalHost && IPlist.at(i).toIPv4Address())
        {
            ui->IPlineEdit->setText(IPlist.at(i).toString());
            break;
        }
    }
}

void MainWindow::port_bind()
{
    int port = ui->PortlineEdit->text().toInt();
    bool flag = msocket->bind(port);
    if(flag)
    {
        //绑定成功(“绑定”按钮设置为失效，“端口号”输入框设置为只读)
        ui->BindBtn->setEnabled(false);
        ui->PortlineEdit->setReadOnly(true);
        ui->cancel_bind_Btn->setEnabled(true);
        QMessageBox::information(this,"提示","绑定成功！");
    }
    else
    {
        //绑定失败
        QMessageBox::information(this,"提示","绑定失败！");
    }

}

void MainWindow::send_message()
{
    // 获取目标端口号，IP，要发送的文本
    int Aimport = ui->UdpPORTlineEdit->text().toInt();
    QString Aimip = ui->UdpIPlineEdit->text();
    QString sendText = ui->SendtextEdit->toPlainText();

    //用utf8编码
    msocket->writeDatagram(sendText.toUtf8(),QHostAddress(Aimip),Aimport);
}

void MainWindow::recv_message()
{
    //接收发送者的IP和端口号
    QHostAddress AimIP;
    quint16 AimPort;    //无符号16位整数

    while(msocket->hasPendingDatagrams())
    {

        QByteArray datagram;
        datagram.resize(msocket->pendingDatagramSize());

        //读取接收信息，IP和端口号
        msocket->readDatagram(datagram.data(),datagram.size(),&AimIP,&AimPort);

        // 更新总接收字节数
        totalBytesReceived += datagram.size();

        // 显示总字节数
        ui->totalByte->setText(QString("总接收字节数：%1字节").arg(totalBytesReceived));

        //转化为可显示文本
        QString buf;
        buf = QString::fromUtf8(datagram); // 用utf8解码]

        // 将接收的文本提取数值并保存在列表中
        //qDebug()<<buf;
        IMUData imuData = extract_data(buf);

        //画accx
        QString accx_str = imuData.accx;
        float accx_flo = accx_str.toFloat();
        accx_lineSeries->append(QPointF(pointNum, accx_flo));
        pointNum++;


        //qDebug()<<"解码后："<<buf<<"\n";

        //获取当前时间并转化为固定格式 (年-月-日 小时:分钟:秒)
        QDateTime currentTime = QDateTime::currentDateTime();
        QString Time = currentTime.toString("yyyy-MM-dd HH:mm:ss");
        save_data = save_data +Time;
        save_data = save_data +"\t" + imuData.accx + "\t" + imuData .accy + "\t" +imuData.accz
                    + "\t" + imuData.x + "\t" + imuData.y + "\t" +imuData.z + "\t" + imuData.w
                    + "\t" +imuData.gx + "\t" +imuData.gy + "\t" + imuData.gz;
        save_data += "\n";
        qDebug()<<save_data;

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

void MainWindow::save_message(QString save_data)
{// Excel默认情况下假定文件采用Windows ANSI编码，而不是UTF-8编码,因此中文会乱码
    QString currentPath = QDir::currentPath();
    QString title = "另存为一个文件";

    QString filename = QFileDialog::getSaveFileName(this,tr("将接收的文本数据另存为"),currentPath,tr("*.xlsx"));
//        QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
//                                       "/home/jana/untitled.png",
//                                       tr("Images (*.png *.xpm *.jpg)"));
    if(filename.isEmpty())
    {
        return;
    }
    else
    {
        QFile file(filename);
        file.open(QIODevice::WriteOnly|QIODevice::Text); // 确保文件以UTF8编码模式打开
        QByteArray save_data_1 = save_data.toUtf8();
        file.write(save_data_1,save_data_1.length());
        QMessageBox::information(this,"提示","保存成功！");
        file.close();
    }
}

void MainWindow::initExcel()
{
    save_data = "imu\n";
    save_data =save_data +"recv_time" +"\t" + "AccX" + "\t" + "AccY" +"\t" + "AccZ" +"\t" +
                "W0" + "\t"+"W1" +"\t" + "W2" +"\t" + "W3" + "\t" +
                "GyroX" + "\t"+"GyroY" +"\t" + "GyroZ" +"\n";
}

MainWindow::IMUData MainWindow::extract_data(const QString& jsonData)
{
    IMUData imuData;
    // 解析JSON数据
    QJsonParseError jsonError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData.toUtf8(), &jsonError);

    if (!jsonDoc.isNull()) {
        if (jsonDoc.isObject()) {
            QJsonObject jsonObj = jsonDoc.object();
            QJsonValue oValue = jsonObj.value("o");
            QJsonObject oObject = oValue.toObject();

            imuData.x = oObject.value("x").toString();
            imuData.y = oObject.value("y").toString();
            imuData.z = oObject.value("z").toString();
            imuData.w = oObject.value("w").toString();
            imuData.accx = oObject.value("accx").toString();
            imuData.accy = oObject.value("accy").toString();
            imuData.accz = oObject.value("accz").toString();
            imuData.gx = oObject.value("gx").toString();
            imuData.gy = oObject.value("gy").toString();
            imuData.gz = oObject.value("gz").toString();
            imuData.s = oObject.value("s").toString();
            imuData.p = oObject.value("p").toString();
        }
    }

    return imuData;
}



// 初始化图表
void MainWindow::initCharts()
{
    // 1. 创建对象
    chartView = new QChartView(ui->ACCview);
    acc_Chart = new QChart;
    m_axisX = new QValueAxis;
    m_axisY = new QValueAxis;

    // 2.设置横纵坐标
    // 2.1 设置坐标轴名称和所属
    m_axisX->setTitleText("时间");
    m_axisY->setTitleText("加速度");
    acc_Chart->addAxis(m_axisX,Qt::AlignBottom);
    acc_Chart->addAxis(m_axisY,Qt::AlignLeft);

    // 2.2 设置坐标轴名称字体样式
    QFont font_x("黑体", 12);
    m_axisX->setTitleFont(font_x);
    QFont font_y("黑体", 12);
    m_axisY->setTitleFont(font_y);

    // 2.3. 设置坐标轴取值范围
    m_axisX->setRange(0,1000);
    m_axisY->setRange(-5,5);

    // 3.显示坐标轴
    chartView->setChart(acc_Chart);
    //chartView->setGeometry(ui->ACCview);
    int x = ui->ACCview->x();
    int y = ui->ACCview->y();
    int width = ui->ACCview->width();
    int height = ui->ACCview->height();
    chartView->setGeometry(x, y, width, height);

    // 4. 设置曲线参数
    QPen pen;
    pen.setWidth(0.5);
    pen.setColor(Qt::black);  // 设置线条颜色
    pen.setStyle(Qt::SolidLine);  // 设置线条样式为实线

    accx_lineSeries = new QLineSeries();
    accx_lineSeries->setPen(pen);
    accx_lineSeries->setPointsVisible(false);                         // 设置数据点可见
    accx_lineSeries->setName("加速度");
    acc_Chart->addSeries(accx_lineSeries);
    accx_lineSeries->attachAxis(m_axisX);                             // 曲线对象关联上X轴，此步骤必须在m_chart->addSeries之后
    accx_lineSeries->attachAxis(m_axisY);

}




void MainWindow::on_cancel_bind_Btn_clicked()
{
    msocket->close();
    ui->BindBtn->setEnabled(true);
    ui->PortlineEdit->setReadOnly(false);
    ui->cancel_bind_Btn->setEnabled(false);
    QMessageBox::information(this,"提示","取消绑定!");
}

