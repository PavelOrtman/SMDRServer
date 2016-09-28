#include "infosocket.h"

InfoSocket::InfoSocket(int sd, QObject *parent) : QObject(parent)
{
    socketDescriptor = sd;
}

InfoSocket::~InfoSocket()
{
    qDebug() << __FILE__ << __LINE__;
}

void InfoSocket::exec(){
    socket = new QTcpSocket(this);
    QObject::connect(socket.data(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
    socket.data()->setSocketDescriptor(socketDescriptor);
    socket.data()->waitForConnected(5000);

    lastMessage = QTime::currentTime().toString("[hh:mm:ss] ");
    lastMessage.append("-= Welcome =-");
    lastMessage.append("\n\r");
    socket.data()->write(lastMessage.toAscii());
    socket.data()->waitForBytesWritten(1000);    
}

void InfoSocket::setMessage(QString str){
    lastMessage = QTime::currentTime().toString("[hh:mm:ss] ");
    lastMessage.append(str);
    lastMessage.append("\n\r");

    socket.data()->write(lastMessage.toAscii());
    socket.data()->waitForBytesWritten(1000);
}

void InfoSocket::socketStateChanged(QAbstractSocket::SocketState state) {
    if (state != QAbstractSocket::ConnectedState && state != QAbstractSocket::ConnectingState) {
        emit this->finished();
    }
}

