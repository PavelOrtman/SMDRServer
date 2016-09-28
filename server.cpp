#include "server.h"

Server::Server(QObject *parent) : QObject(parent)
{
    // меняем директорию на директорию из которой нас запустили
    QString path;
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "SilverSoft", "SMDRServer");
    settings.beginGroup("Stuff");
    path = settings.value("Path", "C:/Server").toString();
    settings.endGroup();
    QDir::setCurrent(path);


    if (this->connectToDB()) {
        socket = new QTcpSocket;
        QObject::connect(socket.data(), SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(socketStateChanged(QAbstractSocket::SocketState)));
        QObject::connect(socket.data(), SIGNAL(readyRead()), this, SLOT(readyRead()));

        socket.data()->connectToHost(QHostAddress("192.168.0.114"), 9000);

        QObject::connect(this, SIGNAL(dataReaded(QByteArray)), this, SLOT(parseString(QByteArray)), Qt::QueuedConnection);

        infoserver = new InfoServer(this);
        QObject::connect(infoserver.data(), SIGNAL(clientConnected(int)), this, SLOT(clientConnected(int)));
        if (!infoserver->listen(QHostAddress::Any, 9000)) {
            //
        }
        QTimer::singleShot(1000 * 60, this, SLOT(checkConnection()));
    }
}

Server::~Server()
{

}

void Server::terminate()
{
    socket.data()->blockSignals(true);
    socket.data()->disconnectFromHost();
    socket.data()->waitForDisconnected(30000);
}

bool Server::connectToDB()
{
    mainDB = QSqlDatabase::addDatabase("QSQLITE", "Main");
    mainDB.setDatabaseName("smdr.db3");
    if (!mainDB.open()) {
        return false;
    }
    return true;
}

QString Server::getCOLineNumber(int i)
{
    if (i == 7)          return "28-72-94";
    else if (i == 8)     return "28-73-44 OMTC";
    else if (i == 9)     return "28-72-97";
    else if (i == 10)    return "28-73-00 Cбыт";
    else if (i == 11)    return "28-72-82 глИнж";
    else if (i == 12)    return "28-72-76 Самов";
    else if (i == 13)    return "28-72-95";
    else if (i == 14)    return "28-73-79";
    else if (i == 15)    return "28-74-63";
    else if (i == 16)    return "28-73-00 Cбыт";     // копия 10
    else if (i == 17)    return "28-73-70 Дисп";
    else if (i == 18)    return "28-73-71 ПДО";
    else if (i == 19)    return "Город гл.Бух";
    else if (i == 20)    return "28-73-58";
    else if (i == 21)    return "";
    else if (i == 22)    return "";
    else                 return "-";
}

void Server::socketStateChanged(QAbstractSocket::SocketState state)
{
    if (state != QAbstractSocket::ConnectedState && state != QAbstractSocket::ConnectingState) {
        QTimer::singleShot(5000, this, SLOT(reconnect()));
    }
}

void Server::reconnect(){
    if (socket.data()->state() != QAbstractSocket::ConnectedState) {
        socket.data()->connectToHost(QHostAddress("192.168.0.114"), 9000);
        QTimer::singleShot(5000, this, SLOT(reconnect()));
    }
}

void Server::readyRead()
{
    lastMessage = QTime::currentTime();    
    QByteArray inData(socket.data()->readAll());

    emit this->dataReaded(inData);    
}

void Server::parseString(QByteArray in)
{
    QSqlQuery query(mainDB);    
    int internalNumber;
    int COLine;

    QDateTime dayTime;
    QTime duration;
    QString outLine;


    internalNumber = in.mid(0, 3).toInt();    
    if (internalNumber == 0) return;

    COLine = in.mid(6, 3).toInt();
    duration = QTime::fromString(in.mid(10, 8), "hh:mm:ss");
    dayTime.setDate(QDate::fromString(in.mid(19, 6) + "20" + in.mid(25, 2), "dd/MM/yyyy"));
    dayTime.setTime(QTime::fromString(in.mid(28, 5), "hh:mm"));    
    outLine = in.mid(34, 15);

    if (outLine.left(1) == "E") {         /// internal --> internal
        emit this->setMessage(QString::number(internalNumber) + "     --> " + outLine.mid(1, 3) + "; " + duration.toString("mm:ss") + ";");        
        query.prepare("INSERT INTO log (daytime, fromline, toline, duration) VALUES (:dt, :f, :t, :d)");
        query.bindValue(":dt", dayTime.toUTC());
        query.bindValue(":f", QString::number(internalNumber));
        query.bindValue(":t", outLine.mid(1, 3));
        query.bindValue(":d", duration);
        if (!query.exec()) {
            emit this->setMessage("DB error " + query.lastError().text());
        }
    }else if (outLine.left(1) == "O") {   /// internal --> external
        int space(outLine.indexOf(" "));
        emit this->setMessage(QString::number(internalNumber) + "[" + this->getCOLineNumber(COLine) + "] --> " + outLine.mid(1, space-1) + "; " + duration.toString("mm:ss") + ";");        
        query.prepare("INSERT INTO log (daytime, fromline, toline, co, duration) VALUES (:dt, :f, :t, :co, :d)");
        query.bindValue(":dt", dayTime.toUTC());
        query.bindValue(":f", QString::number(internalNumber));
        query.bindValue(":t", outLine.mid(1, space-1));
        query.bindValue(":co", COLine);
        query.bindValue(":d", duration);
        if (!query.exec()) {
            emit this->setMessage("DB error " + query.lastError().text());
        }
    }else if (outLine.left(1) == "I") {   /// external --> internal
        emit this->setMessage(this->getCOLineNumber(COLine) + "       --> " + QString::number(internalNumber) + "; " + duration.toString("mm:ss") + ";");        
        query.prepare("INSERT INTO log (daytime, fromline, toline, co, duration) VALUES (:dt, :f, :t, :co, :d)");
        query.bindValue(":dt", dayTime.toUTC());
        query.bindValue(":f", "I");
        query.bindValue(":t", QString::number(internalNumber));
        query.bindValue(":co", COLine);
        query.bindValue(":d", duration);
        if(!query.exec()){
            emit this->setMessage("DB error " + query.lastError().text());
        }
    }    
}

void Server::clientConnected(int sd){    
    QThread *thread = new QThread;
    InfoSocket *s = new InfoSocket(sd);
    s->moveToThread(thread);

    QObject::connect(thread, SIGNAL(started()), s, SLOT(exec()));
    QObject::connect(s, SIGNAL(finished()), thread, SLOT(quit()));
    QObject::connect(s, SIGNAL(finished()), s, SLOT(deleteLater()));
    QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    QObject::connect(this, SIGNAL(setMessage(QString)), s, SLOT(setMessage(QString)));

    thread->start();
}

void Server::checkConnection(){
    if (lastMessage.secsTo(QTime::currentTime()) > 60 * 10) {
        socket.data()->close();
        socket.data()->disconnectFromHost();
        socket.data()->waitForDisconnected();
    }
    QTimer::singleShot(1000 * 60, this, SLOT(checkConnection()));
}

