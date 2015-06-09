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
            if(request.startsWith("gettrainingset"))
            {
                QFile currentJob(Util::getLineFromConf("pathToJobs"));
                if(currentJob.open(QFile::ReadOnly))
                {
                    write("trainingset " + currentJob.readAll());
                }
                else
                {
                    Util::writeError("Can't open job file : " + currentJob.fileName());
                    write("999");
                }
            }
            else if(request.startsWith("getbrain"))
            {
                write("brain " + DatabaseManager::getLastBrain());
            }
            else if(request.startsWith("sendbrain"))
            {
                mongo::BSONObj receivedBrain= fromjson(request.remove(0,9).toUtf8());
                if(receivedBrain.hasField("ratio"))
                {
                    qDebug() << QString::fromStdString(receivedBrain.getField("ratio").toString());
                    qDebug() <<">";
                    qDebug() <<DatabaseManager::getAverageRatio();
                    if(receivedBrain.getField("ratio").numberDouble() > DatabaseManager::getAverageRatio())
                    {
                        DatabaseManager::saveBestBrain(QString::fromStdString(receivedBrain.jsonString()));
                    }
                    DatabaseManager::saveBrain(QString::fromStdString(receivedBrain.jsonString()));
                }
                write("brainreceived");
            }
            else if(request.startsWith("getjobid"))
            {
                write("jobid -1");
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
    Util::write(request);
    return request;
}

bool SocketThread::write(QString answer)
{
    //Util::write(answer);
    answer += "\r\n";
    if(socketClient->write(answer.toUtf8())!= -1)
    {
        return true;
    }
    else
        return false;
}
