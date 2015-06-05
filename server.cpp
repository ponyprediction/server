#include "server.hpp"
#include <iostream>
#include "util.hpp"
Server::Server()
{

}

Server::~Server()
{
    for(int i = 0 ; i < listSocketThread.size() ; i++)
    {
        listSocketThread[i]->quit();
        while (listSocketThread[i]->isRunning());
        delete listSocketThread[i];
    }
    Util::write("Server successfully close");
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    Util::write("Incoming Connection");
    SocketThread *socketThread = new SocketThread(socketDescriptor
                                                  );
    QObject::connect(socketThread, SIGNAL(finished())
                     ,socketThread, SLOT(deleteLater()));
    QObject::connect(socketThread,SIGNAL(disconnection(SocketThread*))
                     ,this,SLOT(onDisconnection(SocketThread*)));
    listSocketThread.push_back(socketThread);
    socketThread->start();
    Util::write(QString::number(listSocketThread.size()) + " Active Connection");
}

void Server::onDisconnection(SocketThread * currentThread)
{
    Util::write("Outgoing connection");
    if(listSocketThread.indexOf(currentThread) != -1)
        listSocketThread.removeAt(listSocketThread.indexOf(currentThread));
    Util::write(QString::number(listSocketThread.size()) + " Active Connection");
}

