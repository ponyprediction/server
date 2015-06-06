#include <QCoreApplication>
#include "util.hpp"
#include "server.hpp"
#include <signal.h>
#include "database-manager.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DatabaseManager::createFirstBrain();
    Util::initConfigFilePath(&a);
    Server server;
    Util::init(&a,&server);
#ifdef  _WIN32
    Util::writeError("Cannot handle ctrl+C : you're on windows... Sucker !");
#else
    //Util::catchUnixSignals({SIGQUIT, SIGINT, SIGTERM, SIGHUP});
#endif
    if (!server.listen(QHostAddress::Any,
                       Util::getLineFromConf("port").toInt())) {
        Util::writeError("Unable to start the server: " + server.errorString());
        return 0;
    }
    else
    {
        Util::writeSuccess("Server started on : " +
                           QString::number(server.serverPort()) + " !");
    }
    return a.exec();
}
