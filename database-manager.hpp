#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include "mongo/client/dbclient.h"

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
};

#endif // DATABASEMANAGER_H
