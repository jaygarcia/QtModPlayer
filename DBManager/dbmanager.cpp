#include "dbmanager.h"
#include "../modfile.h"


int connectionCount = 0;

DBManager::DBManager(QObject *parent) : QObject(parent)
{
}

void DBManager::purgeCurrentPlaylist() {
    qDebug() << "purgeCurrentPlaylist()";

    this->connect();

    QSqlQuery query(this->m_db);
    query.prepare("delete from playlist_songs where playlist_id = 0");

    if (!query.exec()) {
        qDebug() << "Could not purge current playlist";
        qDebug() << query.lastError();
    }
    query.clear();

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
 while (it.hasNext()) {
  it.next();
  str.replace(it.key(),it.value().toString());
 }
 return str;
}

void DBManager::run() {
    if (! this->connect()) {
        qDebug() << "Cannot connect to database! Cannot run()";
        return;
    }

    int totalDone = 0,
        lastPctDone = 0,
        pctDone = 0;

    float percentDone;


    QSqlQuery query(this->m_db);

    query.prepare(
        "INSERT OR IGNORE INTO playlist_songs (playlist_id, song_name, file_name, full_path)"
        " VALUES (:playlist_id, :song_name, :file_name, :full_path) "
    );

    for (int i = 0; i < m_filesToInsert.size(); ++i) {

        ModFile *modFile = m_filesToInsert.at(i);

        query.bindValue(":playlist_id", 0);
        query.bindValue(":file_name", modFile->file_name);
        query.bindValue(":full_path", modFile->full_path);
//        query.bindValue(":song_name", modFile->song_name);

        if (query.exec()) {
            totalDone++;
            query.finish();
            // Sleep for a tad
            this->thread()->msleep(5);
        }
        else {
            qDebug() << m_filesToInsert.at(i)->file_name << "FAILURE!";
            qDebug() << query.lastError();
            qDebug() << getLastExecutedQuery(query);
        }

        percentDone = (float)totalDone / (float)m_filesToInsert.size();

        pctDone = (int) (percentDone * 100.0);

        if (pctDone > lastPctDone) {
            emit insertPercentUpdate(pctDone);
            lastPctDone = pctDone;
        }

    }

    emit insertComplete(totalDone);

    this->disconnect();
}

void DBManager::addToPlaylist(int playlistId, QVector<ModFile *> filesToInsert) {
    this->m_playlistId = playlistId;
    this->m_filesToInsert = filesToInsert;
}

bool DBManager::connect() {
    if (! this->checkForDeployedDatabase()) {
        qDebug() << "Cannot connect to database!";
        return false;
    }

    QString dbId =  "sql_" + QString::number(connectionCount);

    if (! QSqlDatabase::contains(dbId)) {
        QStringList names = QSqlDatabase::connectionNames();


        qDebug() << ">>>> ADDING DATABASE" << dbId;
        m_db = QSqlDatabase::addDatabase("QSQLITE", dbId);
        m_db.setDatabaseName(this->m_dbPath);
        bool hasOpened = m_db.open();
        qDebug() << "m_db.databaseName =" << m_db.databaseName();

        qDebug() << "hasOpened" << hasOpened;

        ++connectionCount;

        return hasOpened;
    }
    else {
        qDebug() << "HERE HERE HERE";
        QStringList names = QSqlDatabase::connectionNames();
        m_db = QSqlDatabase::cloneDatabase(QSqlDatabase::database("qt_sql_default_connection"), "SQLITE");
        m_db.open();
//        m_db.setDatabaseName(this->m_dbPath);
        qDebug() << "m_db.databaseName =" << m_db.databaseName();

        qDebug() << "hasOpened" << m_db.isOpen();

        bool wasOpen = m_db.isOpen();

        return wasOpen;
    }


    return true;

}

bool DBManager::disconnect() {
    if (m_db.isOpen()) {
        m_db.close();
        qDebug() << "Database isOpen" << m_db.isOpen();
    }

    if (QSqlDatabase::contains("sql_" + QString::number(connectionCount))) {
        QSqlDatabase::removeDatabase("sql_" + QString::number(connectionCount));
    }
    qDebug() << "current database " << this->m_db.databaseName() << "connection name" << this->m_db.connectionName();
//    m_db.removeDatabase("QSQLITE");

    return true;
}



QSqlDatabase DBManager::db() const
{
    return m_db;
}

void DBManager::setDb(const QSqlDatabase &db)
{
    m_db = db;
}

QString DBManager::dbPath() const
{
    return m_dbPath;
}

void DBManager::setDbPath(const QString &dbPath)
{
    m_dbPath = dbPath;
}

QVector<ModFile *> DBManager::filesToInsert() const
{
    return m_filesToInsert;
}

void DBManager::setFilesToInsert(const QVector<ModFile *> &filesToInsert)
{
    m_filesToInsert = filesToInsert;
}

int DBManager::playlistId() const
{
    return m_playlistId;
}

void DBManager::setPlaylistId(int playlistId)
{
    m_playlistId = playlistId;
}
