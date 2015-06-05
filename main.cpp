#include <QCoreApplication>
#include "util.hpp"
#include "server.hpp"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    Server server;
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
