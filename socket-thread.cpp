#include "socket-thread.hpp"
#include "qendian.h"
#include "util.hpp"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QFile>

SocketThread::SocketThread(int socketDescriptor):
    socketClient(new QTcpSocket())
{
    socketClient->setSocketDescriptor(socketDescriptor);
    logged = false;
    write("hi");
}

SocketThread::~SocketThread()
{
    socketClient->deleteLater();
    emit disconnection(this);
}

void SocketThread::run()
{
    connect(socketClient, SIGNAL(readyRead()), this, SLOT(readyRead()),
            Qt::DirectConnection);
    connect(socketClient, SIGNAL(disconnected()), this, SLOT(disconnect()));
    exec();
}

void SocketThread::readyRead()
{
    QString request = read();
    Util::write(request);
    if (request == "exit")
    {
        write("bye");
        disconnect();
    }
    else if (!logged)
    {
        if(request.startsWith("log"))
        {
            write("welcome");
            logged = true;
        }
        else
        {
            write("wtf");
        }
    }
    else if(logged)
    {
        if(request == "get-job")
        {
            QFile currentJob(Util::getLineFromConf("pathToJobs"));
            if(currentJob.open(QFile::ReadOnly))
            {
                write("job " + currentJob.readAll());
            }
            else
            {
                Util::writeError("Can't open job file : " + currentJob.fileName());
                write("999");
            }
        }
        else
        {
            write("wtf");
        }
    }
    else
    {
        write("wtf");
    }
}

void SocketThread::disconnect()
{
    quit();
}

QString SocketThread::read()
{
    QString request(socketClient->readAll());
    request = request.left(request.size()-2);
    return request;
}

bool SocketThread::write(QString answer)
{
    answer += "\r\n";
    if(socketClient->write(answer.toUtf8())!= -1)
    {
        return true;
    }
    else
        return false;
}
