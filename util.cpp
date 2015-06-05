#include "util.hpp"
#include <QFile>
#include <QXmlStreamReader>
#include <iostream>
#include <QFileInfo>
#include <QDir>


bool Util::writeEnabled = true;
bool Util::overwriteEnabled = true;
bool Util::successEnabled = true;
bool Util::successOverwriteEnabled = true;
bool Util::warningsEnabled = true;
bool Util::warningOverwriteEnabled = true;
bool Util::errorEnabled = true;
bool Util::errorOverwriteEnabled = true;
bool Util::isOverwriting = false;


void Util::init()
{
    //
    bool ok = true;
    QStringList paths;
    //
    if(ok)
    {
        writeEnabled = getLineFromConf("writeEnabled").toInt();
        overwriteEnabled = getLineFromConf("overwriteEnabled").toInt();
        successEnabled = getLineFromConf("successEnabled").toInt();
        successOverwriteEnabled = getLineFromConf(
                    "successOverwriteEnabled").toInt();
        warningsEnabled = getLineFromConf("warningsEnabled").toInt();
        warningOverwriteEnabled = getLineFromConf(
                    "warningOverwriteEnabled").toInt();
        errorEnabled = getLineFromConf("errorEnabled").toInt();
        errorOverwriteEnabled = getLineFromConf("errorOverwriteEnabled").toInt();
    }
    //
    if(ok)
    {
        paths << Util::getLineFromConf("pathToPonyPrediction", &ok) + "/brains";
    }
    //
    if(ok)
    {
        foreach(QString path, paths)
        {
            if(!Util::createDir(path))
            {
                ok = false;
                Util::writeError("cannot create " + path);
                break;
            }
        }
    }
    //
    if(!ok)
    {
        throw std::runtime_error("Could not properly initialize the program.");
    }
}


QString Util::getLineFromConf(const QString & id, bool * ok)
{
    QString output = "";
    QFile file("./conf.xml");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Util::writeError("can not find the conf file "
                         + QFileInfo(file).absoluteFilePath());
        return QString();
    }
    QXmlStreamReader xml(&file);
    while (!xml.atEnd())
    {
        QXmlStreamReader::TokenType token = xml.readNext();
        if(token == QXmlStreamReader::StartElement)
        {
            if(xml.name() == id)
            {
                output = xml.readElementText();
            }
        }
    }
    if(!output.size())
    {
        *ok = false;
        Util::writeError("can not find config line <" + id + ">");
    }
    return output;
}


void Util::showFile(const QString & path)
{
    QFile file(path);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Util::write(file.readAll());
    }
    else
    {
        Util::writeError("can not find " + QFileInfo(file).absoluteFilePath());
    }
}


bool Util::createDir(const QString & path)
{
    bool ok = true;
    QDir dir(path);
    if (!dir.exists())
    {
        ok = dir.mkpath(".");
    }
    return ok;
}


void Util::write(const QString & message)
{
    if(isOverwriting)
    {
        overwrite("");
        isOverwriting = false;
    }
    std::cout << message.toStdString() << std::endl;
}


void Util::writeSuccess(const QString &message)
{
    if(successEnabled)
        write(VERT + message + RESET);
}


void Util::writeWarning(const QString &message)
{
    if(warningsEnabled)
        write(QString(JAUNE) + "Warning : " + message + RESET);
}


void Util::writeError(const QString & message)
{
    if(errorEnabled)
        write(QString(ROUGE) + "Error : " + message + RESET);
}


void Util::overwrite(const QString &message)
{
    if(overwriteEnabled)
    {
        isOverwriting = true;
        std::cout << '\r';
        for(int i = 0 ; i < 80 ; i++)
        {
            std::cout << ' ';
        }
        std::cout << '\r' << message.toStdString() << std::flush;
    }
    else
    {
        write(message);
    }
}


void Util::overwriteSuccess(const QString &message)
{
    if(successEnabled && successOverwriteEnabled)
        overwrite(QString(VERT) + message + QString(RESET));
    else
        writeSuccess(message);
}


void Util::overwriteWarning(const QString &message)
{
    if(warningsEnabled && warningOverwriteEnabled)
        overwrite(QString(JAUNE)  +  "Warning : " + message + QString(RESET));
    else
        writeWarning(message);
}


void Util::overwriteError(const QString &message)
{
    if(warningsEnabled && warningOverwriteEnabled)
        overwrite(QString(JAUNE)  +  "Error : " + message + QString(RESET));
    else
        writeWarning(message);
}
