#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTimer>
#pragma execution_character_set("utf-8") //防止中文乱码

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    void displayHeatImg();

private:
    Ui::Widget *ui;
    QTcpSocket *m_tcpClient = nullptr;
    QByteArray dataBuffer;
    QTimer *m_timer;

private slots:
    void on_connected();
    void on_disconnected();
    void on_readyRead();
    void on_displayPreImg(const QImage& image);
    void on_but_connect_clicked();
    void on_but_send_clicked();
    void on_displayConImg_clicked();
signals:
    void imageReceived(const QImage& image);
};

#endif // WIDGET_H






















