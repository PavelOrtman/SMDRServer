#ifndef INFOSERVER_H
#define INFOSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class InfoServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit            InfoServer(QObject *parent = 0);
    ~InfoServer();

protected:
    void                incomingConnection(int socketDescriptor);

signals:
    void                clientConnected(int sd);
};

#endif // INFOSERVER_H
