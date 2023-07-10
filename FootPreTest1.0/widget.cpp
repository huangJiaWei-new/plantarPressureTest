#include "widget.h"
#include "ui_widget.h"
#include <QColor>
#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    m_tcpClient = new QTcpSocket(this);
    //m_timer = new QTimer(this);
    //m_timer->setInterval(400); //定时400ms

    //TCP连接成功后发出connected信号
    connect(m_tcpClient, &QTcpSocket::connected,    this, &Widget::on_connected);
    //TCP断开连接后发出disconnected信号
    connect(m_tcpClient, &QTcpSocket::disconnected, this, &Widget::on_disconnected);
    //当有可读数据时发出readyRead信号
    connect(m_tcpClient, &QTcpSocket::readyRead,    this, &Widget::on_readyRead);
    //显示压力照片
    //connect(m_timer, &QTimer::timeout, this, &Widget::on_readyRead);
}

Widget::~Widget()
{
    if(m_tcpClient->state() != QAbstractSocket::UnconnectedState)
    {
        m_tcpClient->abort(); //因为再关闭时会修改but_connect文本，所以如果由系统再析构时自动释放则会出现的错误，再这里关闭了就不会出现了
    }
    delete ui;
}


/*--------------@brief：生成热力图像-------------*/
void Widget::displayHeatImg()
{
    QImage grayscaleImage("D:/plantarPressureTest/FootPressureUpperComputer/rst/1.jpg");
    QImage heatmapImage(grayscaleImage.size(), QImage::Format_RGB32);

    //定义热力图的颜色范围，从红色到蓝色过渡
    const double hueStart = 240.0; //蓝色
    const double hueEnd   = 0.0;   //红色
    for(int y = 0; y < grayscaleImage.height(); ++y) //遍历灰度图像的每个像素
    {
        for(int x = 0; x < grayscaleImage.width(); ++x)
        {     
            QRgb grayscaleValue = grayscaleImage.pixel(x, y); //获取当前像素的灰度值
            double normalizedValue = qRed(grayscaleValue) / 255.0; //将灰度值映射到0-1的范围
            double hue = hueStart + (hueEnd - hueStart) * normalizedValue; //计算当前像素的色相值，高灰度低色相，低灰度高色相
            QRgb heatmapValue = QColor::fromHsvF(hue / 360.0, 1.0, 1.0).rgb(); //将色相值转换为RGB颜色
            heatmapImage.setPixel(x, y, heatmapValue); //在热力图像中设置对应像素的颜色
        }
    }

    QPainter painter(&heatmapImage);
    painter.setCompositionMode(QPainter::CompositionMode_Multiply);
    painter.drawImage(0, 0, heatmapImage);

    heatmapImage.save("D:/plantarPressureTest/FootPressureUpperComputer/2.png"); //保存图像
}


/*--------------@brief：TCP连接成功，修改按键状态-------------*/
void Widget::on_connected()
{
    ui->but_connect->setText("断开连接");
}


/*--------------@brief：TCP断开连接，修改按键状态-------------*/
void Widget::on_disconnected()
{
    ui->but_connect->setText("连接");
}


/*--------------@brief：TCP读取数据并显示-------------*/
void Widget::on_readyRead()
{
    while(m_tcpClient->bytesAvailable() >= 480) //等待接收的字节数达到480byte
    {
        QByteArray newData = m_tcpClient->read(480); //每次读取是否是按顺序依次读取480byte,还是每次依旧读取开始的480byte字节
        dataBuffer.append(newData);

        if(dataBuffer.size() >= 120960)
        {
            //qDebug() << "已成功接收120960个byte数据";
            //ui->text_recv->append(dataBuffer); //显示读取到的数据
            QByteArray imgData = dataBuffer.left(120960); //将'dataBuffer'的钱120960个字节赋值给'imgData'
            dataBuffer.remove(0, 120960); //删除'dataBuffer'中0-120960范围内的数据

            QImage img(reinterpret_cast<const uchar*>(imgData.data()), 252, 480,
                       QImage::Format_Grayscale8); //创建灰度图像，图像大小高度480，宽度252
            img = img.transformed(QTransform().rotate(-90)); //让生成的图片逆时针旋转90°

            ui->preImg->setPixmap(QPixmap::fromImage(img));  //在label控件显示压力图像
        }
    }
}


/*--------------@brief：显示压力照片-------------*/
void Widget::on_displayPreImg(const QImage &image)
{
    ui->preImg->setPixmap(QPixmap::fromImage(image));

}


/*--------------@brief：TCP连接/断开控制，不需要指定本地端口，默认自动分配一个端口-------------*/
void Widget::on_but_connect_clicked()
{
    if(m_tcpClient->state() == QAbstractSocket::UnconnectedState) //判断连接状态，如果未连接则开始连接
    {
        m_tcpClient->connectToHost(ui->line_peerAddress->text(),  //目标IP地址
                                   ui->spin_peerPort->value());   //目标端口号
    }
    else //如果不是未连接则断开连接
    {
        m_tcpClient->abort();
    }
}


/*--------------@brief：发送数据-------------*/
void Widget::on_but_send_clicked()
{
    if(m_tcpClient->state() != QAbstractSocket::ConnectedState) return; //判断TCP是否已连接，如果没有连接则不发送数据

    QString str = ui->text_send->toPlainText();    //获取输入框的内容
    qint64 len = m_tcpClient->write(str.toUtf8()); //发送数据，toUtf8()是为了支持发送中文，但是如果接收端编码方式不同则中文可能显示乱码
    if(len < 0)
    {
        qWarning() <<"发送失败！";
    }

    //m_timer->start(); //启动定时器
}


/*--------------@brief：显示等高线图像-------------*/
void Widget::on_displayConImg_clicked()
{
    Widget::displayHeatImg();
}























