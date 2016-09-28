#ifndef INFOSOCKET_H
#define INFOSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QPointer>
#include <QTimer>
#include <QCoreApplication>
#include "windows.h"
#include <QDate>
#include <QTime>

class InfoSocket : public QObject
{
    Q_OBJECT

    QPointer<QTcpSocket>    socket;
    int                     socketDescriptor;

    QString                 lastMessage;

public:
    explicit                InfoSocket(int sd, QObject *parent = 0);
    ~InfoSocket();

public slots:
    void                    exec();
    void                    setMessage(QString str);

private slots:
    void                    socketStateChanged(QAbstractSocket::SocketState state);

signals:
    void                    finished();
};

#endif // INFOSOCKET_H
