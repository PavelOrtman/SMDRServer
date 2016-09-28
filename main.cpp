#define CONSOLE
//#define DAEMON


#ifdef CONSOLE
#include <QTextCodec>
#include "server.h"
#include <QCoreApplication>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);    

    Server s;
    return a.exec();
}
#endif

#ifdef DAEMON
#include "qtservice.h"
#include "daemon.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QTextCodec *codec=QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);

    Daemon serverApplication(argc, argv);
    return serverApplication.exec();
}
#endif
