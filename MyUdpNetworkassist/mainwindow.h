#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QtNetwork>
#include <QIntValidator>
#include <QDateTime>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_BindBtn_clicked();

    void on_SendBtn_clicked();

    void readyRead_Slot();

private:
    Ui::MainWindow *ui;
    QUdpSocket* msocket;
    QIntValidator *PORTvalidator;
};
#endif // MAINWINDOW_H
