#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtNetwork>
#include <QIntValidator>
#include <QDateTime>
#include<QDir>
#include<QFileDialog>
#include<QFile>

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

private:
    Ui::MainWindow *ui;
    QUdpSocket* msocket;
    QIntValidator *PORTvalidator;
};
#endif // MAINWINDOW_H
