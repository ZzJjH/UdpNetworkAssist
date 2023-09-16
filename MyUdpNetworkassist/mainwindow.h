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
    void save_message();

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_BindBtn_clicked();

    void on_SendBtn_clicked();

    void readyRead_Slot();

    void on_saveBtn_clicked();

    void RecvRate_slot();

private:
    Ui::MainWindow *ui;
    QUdpSocket* msocket;
    QIntValidator *PORTvalidator;
    QTimer *mtimer;
    qint64 totalBytesReceived = 0; // 总接收字节数
    double receiveRate = 0.0; // 接收速率（字节/秒）
    QDateTime lastUpdateTime; // 上次更新时间
    qint64 lastTotalBytesReceived = 0; // 上次总接收字节数
};
#endif // MAINWINDOW_H
