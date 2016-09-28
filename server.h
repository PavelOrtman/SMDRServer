#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtSql>
#include <QPointer>
#include <QTcpSocket>
#include <QHostAddress>

#include "infoserver.h"
#include "infosocket.h"

class Server : public QObject
{
    Q_OBJECT

    QSqlDatabase            mainDB;

    QPointer<QTcpSocket>    socket;
    bool                    inReadyRead;

    QPointer<InfoServer>    infoserver;

    QTime                   lastMessage;

public:
    explicit                Server(QObject *parent = 0);
    ~Server();

    void                    terminate();


private:
    bool                    connectToDB();
    QString                 getCOLineNumber(int i);

private slots:
    void                    socketStateChanged(QAbstractSocket::SocketState state);
    void                    reconnect();


    void                    readyRead();
    void                    parseString(QByteArray in);

    void                    clientConnected(int sd);

    void                    checkConnection();

signals:
    void                    dataReaded(QByteArray);
    void                    setMessage(QString m);
};

#endif // SERVER_H
