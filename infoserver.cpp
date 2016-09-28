#include "infoserver.h"

InfoServer::InfoServer(QObject *parent) : QTcpServer(parent)
{

}

InfoServer::~InfoServer()
{

}

void InfoServer::incomingConnection(int socketDescriptor){
    emit this->clientConnected(socketDescriptor);
}

