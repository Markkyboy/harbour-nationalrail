#include "database.h"

#include <QDir>
#include <QDebug>
QString const databaseFolder =".harbour-nationalrail";
QString const databaseName ="stationsDB.sqlite";

Database::Database()
{
    openDB();
}

Database::~Database()
{
    db.close();
}

bool Database::openDB()
{

    db = QSqlDatabase::addDatabase("QSQLITE");


    QString databasePath = QDir::homePath() + QDir::separator()+ databaseFolder + QDir::separator()+ databaseName;

    db.setDatabaseName(databasePath);

    return db.open();
}

QSqlDatabase Database::getDB(){
    return db;
}
