#ifndef FOOTPRECLIENT_H
#define FOOTPRECLIENT_H

#include <QWidget>
#include <QTcpSocket>

class myTcpClient
{
public:
    myTcpClient() {}
    void connect(const QString &ipv4, uint16_t port);
    void write(const QString &data);
    void receiveData();

private:
    QTcpSocket *m_tcpClient = nullptr;
    QByteArray dataBuffer;
    quint64 totalBytesReceived;
    QImage image;
};


#endif // FOOTPRECLIENT_H
