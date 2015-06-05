#include "socket-thread.hpp"
#include "qendian.h"
#include "util.hpp"
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDateTime>
#include "database-manager.hpp"

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
    QString requestAll = read();
    Util::write(requestAll);
    QStringList list = requestAll.split("\r\n");
    foreach (QString request, list)
    {
        if (request == "exit")
        {
            write("bye");
            disconnect();
        }
        else if (!logged)
        {
            if(request.startsWith("log"))
            {
                logged = true;
                write("welcome");

            }
            else
            {
                write("wtf");
            }
        }
        else if(logged)
        {
            qDebug() << request;
            if(request == "getjob")
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
            else if(request == "getbrain")
            {
                QFile currentBrain(Util::getLineFromConf("pathToBrains"));
                if(currentBrain.open(QFile::ReadOnly))
                {
                    write("brain " + currentBrain.readAll());
                }
                else
                {
                    Util::writeError("Can't open job file : " + currentBrain.fileName());
                    write("999");
                }
            }
            else if(request.startsWith("sendbrain"))
            {
                //QJsonDocument receivedBrain = QJsonDocument::fromJson(request.remove(0,9).toUtf8());
                //QJsonObject receivedObject = receivedBrain.object();
                QFile currentBrain(Util::getLineFromConf("pathToBrains"));
                QJsonDocument receivedBrain = QJsonDocument::fromJson(currentBrain.readAll());
                QJsonObject receivedObject = receivedBrain.object();
                receivedObject["date"] = QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss-zzz");
                if(receivedObject["ratio"].toDouble() > DatabaseManager::getAverageRatio())
                {
                    DatabaseManager::saveBrain(currentBrain.readAll());
                }
                write("received");
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
