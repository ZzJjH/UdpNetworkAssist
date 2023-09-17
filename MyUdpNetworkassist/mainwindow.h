#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtNetwork>
#include <QIntValidator>
#include <QDateTime>
#include<QDir>
#include<QFileDialog>
#include<QFile>
#include<QTimer>

// 画图头文件
#include <QChart>
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>
#include <QtMath>
#include <QSplineSeries>
QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    void default_ip_port();
    void port_bind();
    void send_message();
    void recv_message();
    void save_message(QString save_data);
    void initExcel();

    struct IMUData {
        QString x;
        QString y;
        QString z;
        QString w;
        QString accx;
        QString accy;
        QString accz;
        QString gx;
        QString gy;
        QString gz;
        QString s;
        QString p;
    };
    IMUData extract_data(const QString& jsonData);

    //TEMP
    void initCharts();
    QChartView *chartView;
    QChart *acc_Chart;
    QValueAxis *m_axisX, *m_axisY;
    QLineSeries* accx_lineSeries;
    int pointNum = 0;



    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_BindBtn_clicked();

    void on_SendBtn_clicked();

    void readyRead_Slot();

    void on_saveBtn_clicked();

    void RecvRate_slot();

    void on_cancel_bind_Btn_clicked();

private:
    Ui::MainWindow *ui;
    QUdpSocket* msocket;
    QIntValidator *PORTvalidator;
    QTimer *mtimer;
    qint64 totalBytesReceived = 0; // 总接收字节数
    double receiveRate = 0.0; // 接收速率（字节/秒）
    QDateTime lastUpdateTime; // 上次更新时间
    qint64 lastTotalBytesReceived = 0; // 上次总接收字节数
    QString save_data; // 要保存的数据
};
#endif // MAINWINDOW_H
