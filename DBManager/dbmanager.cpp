#include "dbmanager.h"

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    this->purgeCurrentPlaylist();
}

void DBManager::purgeCurrentPlaylist() {
    this->connect();
    qDebug() << "purgeCurrentPlaylist()";

    QSqlQuery query;
    query.prepare("delete from playlist_songs where playlist_id = 0");

    if (!query.exec()) {
        qDebug() << "Could not purge current playlist";
        qDebug() << query.lastError();
    }


    this->disconnect();
}

bool DBManager::checkForDeployedDatabase() {
    QString dbFileName  = "modmusic.db",
            sourceDbFile = QDir::currentPath() + "/../Resources/db/" + dbFileName;

    QString destDbDir = QDir::homePath() + "/.QtModPlayer/",
            destDbFile  = destDbDir + dbFileName;

    this->m_dbPath = destDbFile;

//    qDebug() << "this->m_dbPath =" << destDbFile;
//    qDebug() << destDbFile;

    if (! QFile::exists(destDbDir) || ! QFile::exists(destDbFile)) {
        QDir homeDir = QDir::home();

        if (! homeDir.exists(".QTModPlayer") && ! homeDir.mkdir(".QtModPlayer")) {
            qDebug() << "Cannot make directory :: " << destDbDir;
            return false;
        }

        // debug only
        QFile::remove(destDbFile);

        if (! QFile::exists(destDbFile) && ! QFile::copy(sourceDbFile, destDbFile)) {
            qDebug() << "Cannot make file :: " << destDbFile;
            return false;
        }
    }

    return true;
}

QString getLastExecutedQuery(const QSqlQuery& query)
{
 QString str = query.lastQuery();
 QMapIterator<QString, QVariant> it(query.boundValues());
 while (it.hasNext())
 {
  it.next();
  str.replace(it.key(),it.value().toString());
 }
 return str;
}

void DBManager::addToPlaylist(QString playlistName, QVector<ModFile *> filesToInsert) {
    if (! this->connect()) {
        qDebug() << "Cannot connect to database!";
    }

    qDebug() << "addToPlaylist -- " << playlistName;


    for (int i = 0; i < filesToInsert.size(); ++i) {

        ModFile *modFile = filesToInsert.at(i);

        QSqlQuery query(this->m_db);

        query.prepare(
            "INSERT INTO playlist_songs (playlist_id, file_name, parent_directory)"
            " VALUES (:playlist_id, :file_name, :parent_directory)"
        );


        query.bindValue(":playlist_id", 0);
        query.bindValue(":file_name", modFile->file_name);
        query.bindValue(":parent_directory", modFile->parent_directory);

        if (query.exec()) {
//            qDebug() << filesToInsert.at(i)->file_name << "SUCCESS";
        }
        else {
            qDebug() << filesToInsert.at(i)->file_name << "FAILURE!";
//            qDebug() << query.
            qDebug() << query.lastError();
            qDebug() << getLastExecutedQuery(query);

        }
    }

    this->disconnect();
}

bool DBManager::connect() {
    if (! this->checkForDeployedDatabase()) {
        qDebug() << "Cannot connect to database!";
        return false;
    }

    if (m_db.databaseName().toUtf8() == "") {
        m_db = QSqlDatabase::addDatabase("QSQLITE");
        m_db.setDatabaseName(this->m_dbPath);
    }

    bool hasOpened = m_db.open();

    qDebug() << "m_db.databaseName =" << m_db.databaseName();

    qDebug() << "hasOpened" << hasOpened;



    return hasOpened;
}

bool DBManager::disconnect() {
    qDebug() << "Database disconnected";
    m_db.close();
    return true;
}
