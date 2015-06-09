#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "mongo/client/dbclient.h"
#include <QString>
using namespace mongo;

class DatabaseManager
{
public:
    DatabaseManager();
    ~DatabaseManager();
    static void init();
    static bool initialized;
    static double getAverageRatio();
    static double getBestRatio();
    static void saveBestBrain(const QString &brain);
    static void saveBrain(const QString &brain);
    static QString getLastBrain();
    static void createFirstBrain();
};

#endif // DATABASEMANAGER_H
