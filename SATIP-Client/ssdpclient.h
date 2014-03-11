#ifndef SSDPCLIENT_H
#define SSDPCLIENT_H

#include <QObject>
#include <QAbstractSocket>

class GatewayDevice;
class SSDPClientPrivate;
class SSDPClient : public QObject
{
    Q_OBJECT

public:
    explicit SSDPClient(const QString &address, QObject *parent = 0);
    ~SSDPClient();

    QString localAddress() const;

public slots:
    void searchDevices();

protected:
    bool bind();

protected slots:
    void error(QAbstractSocket::SocketError);
    void readyRead();
    void parseDatagram(QString datagram);
    void parseSsdpNotify(QStringList &tokens);
    void parseSsdpAnswer(QStringList &tokens);

signals:
    void newGatewayDeviceFound(GatewayDevice *device);
    void gatewayDeviceLeft(GatewayDevice *device);

private:
    SSDPClientPrivate *d;
};

#endif // SSDPCLIENT_H
