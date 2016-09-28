#ifndef DAEMON_H
#define DAEMON_H

#include <QtCore>
#include <QSettings>

#include "qtservice.h"
#include "server.h"

class Daemon : public QtService<QCoreApplication>
{
    Server         *server;

public:
    Daemon(int argc, char **argv);
    ~Daemon();

protected:
    void            start();
    void            stop();
};

#endif // DAEMON_H
