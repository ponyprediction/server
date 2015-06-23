#include "database-manager.hpp"
#include "mongo/bson/bson.h"
#include "util.hpp"
#include <iostream>

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
            db.dropCollection("ponyprediction.brains");
            db.createCollection("ponyprediction.brains",5242880,true,100);
        }
    }
}

double DatabaseManager::getAverageRatio()
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
        BSONObj projection = BSON("ratio" << 1);
        BSONObj query;
        if(projection.isValid())
        {
            std::auto_ptr<DBClientCursor> cursor = db.query
                    ("ponyprediction.brains",query,100,0,&projection);
            while(cursor->more())
            {
                BSONObj result = cursor->next();
                if(result.hasField("ratio"))
                {
                    listRatio.push_back(result["ratio"].numberDouble());
                }
                else
                {
                    Util::writeError("No field ratio (getAverageRatio)");
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
    double retour = 0;
    if(listRatio.size() != 0)
    {
        for(int i = 0 ; i < listRatio.size(); i++)
        {
            retour +=  listRatio[i];
        }
        retour /=listRatio.size();
    }
    return retour;
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

void DatabaseManager::saveBestBrain(const QString &brain)
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
        BSONObj brainBson = fromjson(brain.toStdString());
        if(brainBson.isValid())
        {
            db.remove("ponyprediction.bestBrain",Query());
            db.insert("ponyprediction.bestBrain",brainBson);
        }
        else
        {
            Util::writeError("Not valid");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
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
        BSONObj brainBson = fromjson(brain.toStdString());
        if(brainBson.isValid())
        {
            db.insert("ponyprediction.brains",brainBson);
        }
        else
        {
            Util::writeError("Not valid");
        }
    }
    else
    {
        Util::writeError("Not connected to the DB");
    }
}

void DatabaseManager::createFirstBrain()
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
        QFile defaultbrain(Util::getLineFromConf("pathToBrains"));
        if(defaultbrain.open(QFile::ReadOnly))
        {
            BSONObj brain = fromjson(defaultbrain.readAll());
            BSONObjBuilder builder;
            builder.appendElements(brain);
            builder.appendDate("date",0);
            BSONObj brainWithDate = builder.done();
            if(db.count("ponyprediction.bestBrain",brainWithDate) == 0)
            {
                db.insert("ponyprediction.bestBrain",brainWithDate);
            }
        }
        else
        {
            Util::writeError("Can't find the default brain");
        }
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
                    ("ponyprediction.bestBrain",query,1,0,&projection);
            if(cursor->more())
            {
                BSONObj result = cursor->next();
                result = result.removeField("_id");
                retour = QString::fromStdString(result.jsonString());
            }
            else
            {
                createFirstBrain();
                retour = getLastBrain();
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

