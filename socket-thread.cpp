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
    connect(socketClient, SIGNAL(readyRead()), this, SLOT(read()),
            Qt::DirectConnection);
    connect(socketClient, SIGNAL(disconnected()), this, SLOT(disconnect()));
    exec();
}

void SocketThread::handleRequest(QString request)
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
            BSONObjBuilder builder;
            builder.appendElements(receivedBrain);
            builder.appendDate("date",mongo::jsTime());
            BSONObj brainWithDate = builder.done();
            //qDebug() << QString::fromStdString(brainWithDate.jsonString());
            if(brainWithDate.hasField("ratio") && brainWithDate.isValid())
            {
                if(brainWithDate.getField("ratio").numberDouble() > DatabaseManager::getAverageRatio())
                {
                    DatabaseManager::saveBestBrain(QString::fromStdString(brainWithDate.jsonString()));
                }
                DatabaseManager::saveBrain(QString::fromStdString(brainWithDate.jsonString()));
            }
            else
            {
                Util::writeError("Unvalid brainwithDate or no field ratio (saveBrain)");
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

/*void SocketThread::readyRead()
{
    QString requestAll = read();
    QStringList list = requestAll.split("\r\n");
    foreach (QString request, list)
    {
        handleRequest(request);

    }
}*/

void SocketThread::disconnect()
{
    quit();
}

void SocketThread::read()
{
    QString answerTotal = socketClient->readAll();
    QStringList list = answerTotal.split("\r\n");
    for(int i = 0 ; i < list.size() ; i++)
    {
        if(i)
        {
            handleRequest(currentRequest);
            currentRequest = list[i];
        }
        else
        {
            currentRequest += list[i];
        }
    }
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
