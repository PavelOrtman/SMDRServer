#include "daemon.h"
#include <QFile>

Daemon::Daemon(int argc, char **argv) : QtService<QCoreApplication>(argc, argv, "SMDRServer")
{
    this->setServiceDescription("Сервер снятия логов с АТС iPECS-300");
    this->setStartupType(QtServiceController::AutoStartup);
    //this->setServiceFlags(QtServiceBase::NeedsStopOnShutdown); i need this?

    server = 0;
}

Daemon::~Daemon()
{
}

void Daemon::start()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "SilverSoft", "SMDRServer");
    settings.beginGroup("Stuff");
    settings.setValue("Path", QCoreApplication::applicationDirPath());
    settings.endGroup();

    QCoreApplication *app = application();
    server = new Server(app);
}

void Daemon::stop()
{
    server->terminate();
}
