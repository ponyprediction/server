#include "database-manager.hpp"
#include "mongo/bson/bson.h"
#include "util.hpp"

bool DatabaseManager::initialized = false;

DatabaseManager::DatabaseManager()
{

}

DatabaseManager::~DatabaseManager()
{

}

void DatabaseManager::init()
{
    if(!initialized)
    {
        initialized = true;
        mongo::client::initialize();
    }
}

float DatabaseManager::getAverageRatio()
{
    QVector<float> listRatio;
    init();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
                         QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = fromjson("{ratio:-1}");//= BSON();
        Query query;
        if(projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db.query
                    ("ponyprediction.brains",query.sort("date",-1),100,0,&projection);
            while(cursor->more())
            {
                BSONObj result = cursor->next();
                if(result.hasField("ratio"))
                {
                    listRatio.push_back((float)result["ratio"]._numberDouble());
                }
            }
        }
        else
        {
            Util::writeError("Projection is not valid (getTrainerRaceCount)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
    qDebug() << listRatio;
    float retour = 0;
    for(int i = 0 ; i < listRatio.size(); i++)
    {
        retour +=  listRatio[i];
    }
    return retour/listRatio.size();
}


double DatabaseManager::getBestRatio()
{
    double retour = -1;
    init();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
                         QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection = fromjson("{ratio:-1}");//= BSON();
        Query query;
        if(projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db.query
                    ("ponyprediction.brains",query.sort("ratio",-1),1,0,&projection);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                if(result.hasField("ratio"))
                {
                    retour = result["ratio"]._numberDouble();
                }
            }
            else
            {
                //ok = false;
                //error = "No data in database";
            }
        }
        else
        {
            Util::writeError("Projection is not valid (getTrainerRaceCount)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
    return retour;
}

void DatabaseManager::saveBrain(const QString &brain)
{
    init();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
                         QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj brain = fromjson(brain.toString());
        db.insert("ponyprediction.brain",brain);
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
}

QString DatabaseManager::getLastBrain()
{
    QString retour = QString();
    init();
    DBClientConnection db;
    try
    {
        db.connect("localhost");
    }
    catch ( const mongo::DBException &e )
    {
        Util::writeError("Connexion à la DB échoué (DataBaseManager) : " +
                         QString::fromStdString(e.toString()));
    }
    if(db.isStillConnected())
    {
        BSONObj projection;
        Query query;
        if(projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db.query
                    ("ponyprediction.brains",query.sort("date",-1),1,0,&projection);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                retour = QString::fromStdString(result.toString());
            }
        }
        else
        {
            Util::writeError("Projection is not valid (getTrainerRaceCount)");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
    return retour;
}

