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
    static float getAverageRatio();
    static double getBestRatio();
    static void saveBrain(const QString &brain);
    static QString getLastBrain();
};

#endif // DATABASEMANAGER_H
