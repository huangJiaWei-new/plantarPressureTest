#include "footPreClient.h"


/*--------------@brief：TCP连接/断开控制，不需要指定本地端口，默认自动分配一个端口-------------*/
void myTcpClient::connect(const QString &ipv4, uint16_t port)
{
    if(m_tcpClient->state() == QAbstractSocket::UnconnectedState) //判断连接状态，如果未连接则开始连接
    {
        m_tcpClient->connectToHost(ipv4, port);
    }
    else  //如果不是未连接则断开连接
    {
        m_tcpClient->abort();
    }
}


/*--------------@brief：发送数据-------------*/
void myTcpClient::write(const QString &data)
{
    //判断TCP是否已连接，如果没有连接则不发送数据
    if(m_tcpClient->state() != QAbstractSocket::ConnectedState) return;

    //发送数据，toUtf8()是为了支持发送中文，但是如果接收端编码方式不同则中文可能显示乱码
    qint64  len = m_tcpClient->write(data.toUtf8());
    if(len < 0)
    {
        qWarning() <<"发送失败！";
    }
}


/*--------------@brief：接受数据-------------*/
void myTcpClient::receiveData()
{
    while(m_tcpClient->bytesAvailable() > 0)
    {
        QByteArray newData = m_tcpClient->readAll();
        dataBuffer.append(newData);
        if(totalBytesReceived == 120960)
        {
            image = QImage::fromData(dataBuffer, "JPG");//Data received, process and display the image
            emit imageReceived(image);
            //Reset variables for next image
            totalBytesReceived = 0;
            dataBuffer.clear();
        }
    }
}




