#ifndef SOCKETTHREAD_H
#define SOCKETTHREAD_H

#include <QThread>
#include <QTcpSocket>
#include <QNetworkReply>
#include <QFile>

class SocketThread : public QThread
{
    Q_OBJECT
public:
    SocketThread(int socketDescriptor);
    ~SocketThread();
    void run() Q_DECL_OVERRIDE;
public slots:
    void read();
    void disconnect();
signals:
    void disconnection(SocketThread*);
private:
    QTcpSocket *socketClient;
    bool write(QString answer);
    //QString read();
    bool logged;
    void handleRequest(QString request);
    QString currentRequest;
};

#endif // SOCKETTHREAD_H
