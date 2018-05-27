#include "dbmanager.h"
#include "../modfile.h"



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


int connectionCount = 0;

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    m_dbFileName  = "modmusic.db";

    QString destDbDir = QDir::homePath() + "/.QtModPlayer/",
            destDbFile  = destDbDir + m_dbFileName;


    this->m_dbPath = destDbFile;
}

void DBManager::purgeCurrentPlaylist() {
//    qDebug() << "purgeCurrentPlaylist()";

    this->connect();

    QSqlQuery query(this->m_db);
    query.prepare("delete from playlist_songs where playlist_id = 0");

    if (!query.exec()) {
        qWarning() << "Could not purge current playlist";
        qWarning() << query.lastError();
    }
    query.clear();

    this->disconnect();
}

bool DBManager::checkForDeployedDatabase() {
    QString sourceDbFile = QDir::currentPath() + "/../Resources/db/" + m_dbFileName;


    QString destDbDir = QDir::homePath() + "/.QtModPlayer/",
            destDbFile  = destDbDir + m_dbFileName;

//    qDebug() << "this->m_dbPath =" << destDbFile;
//    qDebug() << destDbFile;

    if (! QFile::exists(destDbDir) || ! QFile::exists(destDbFile)) {
        QDir homeDir = QDir::home();

        if (! homeDir.exists(".QTModPlayer") && ! homeDir.mkdir(".QtModPlayer")) {
            qWarning() << "Cannot make directory :: " << destDbDir;
            return false;
        }

        // debug only
        QFile::remove(destDbFile);

        if (! QFile::exists(destDbFile) && ! QFile::copy(sourceDbFile, destDbFile)) {
            qWarning() << "Cannot make file :: " << destDbFile;
            return false;
        }
    }

    return true;
}

void DBManager::bulkInsertToPlaylist() {
    if (! this->connect()) {
        qWarning() << "Cannot connect to database! Cannot bulkInsertToPlaylist()";
        return;
    }

    int totalDone = 0,
        lastPctDone = 0,
        pctDone = 0;

    float percentDone;


    QSqlQuery query(this->m_db);

    query.prepare(
        "INSERT OR IGNORE INTO playlist_songs (playlist_id, song_name, file_name, full_path, was_processed)"
        " VALUES (:playlist_id, :song_name, :file_name, :full_path, :was_processed) "
    );

    for (int i = 0; i < m_filesToInsert.size(); ++i) {

        ModFile *modFile = m_filesToInsert.at(i);

        query.bindValue(":playlist_id", 0);
        query.bindValue(":file_name", modFile->file_name);
        query.bindValue(":full_path", modFile->full_path);

        // Lets grab song info for the first thirty just so it looks prettier
//        if (i < 30) {
            std::ifstream file(modFile->full_path.toUtf8(), std::ios::binary);

            openmpt::module mod(file);

//            printf("%i -- Song title:: %s\n", i, mod.get_metadata("title").c_str());
//            fflush(stdout);

            QString songName = QString::fromUtf8(mod.get_metadata("title").c_str());
//            qDebug() << songName;
            query.bindValue(":was_processed", 1);
            query.bindValue(":song_name", songName);
//        }

        if (query.exec()) {
            totalDone++;
            query.finish();
            // Sleep for a tad
            this->thread()->msleep(1);
        }
        else {
            qWarning() << m_filesToInsert.at(i)->file_name << "FAILURE!";
            qWarning() << query.lastError();
            qWarning() << getLastExecutedQuery(query);
        }

        percentDone = (float)totalDone / (float)m_filesToInsert.size();

        pctDone = (int) (percentDone * 100.0);

//        int modNumber = 0;

//        if (pctDone < 20) {
//            modNumber = pctDone % 1;
//        }
//        else {
//            modNumber = pctDone % 5;
//        }


        if (pctDone > lastPctDone && pctDone % 2 == 0) {
            emit insertPercentUpdate(pctDone);
            lastPctDone = pctDone;
        }

    }

    emit insertComplete(totalDone);

    this->disconnect();
}

void DBManager::queueAddToPlaylist(int playlistId, QVector<ModFile *> filesToInsert) {
    this->m_playlistId = playlistId;
    this->m_filesToInsert = filesToInsert;
}

bool DBManager::connect() {
    if (! this->checkForDeployedDatabase()) {
        qWarning() << "Cannot connect to database!";
        return false;
    }

    QString dbId =  "sql_" + QString::number(connectionCount);

    if (! QSqlDatabase::contains(dbId)) {
        QStringList names = QSqlDatabase::connectionNames();


//        qDebug() << ">>>> ADDING DATABASE" << dbId;
        m_db = QSqlDatabase::addDatabase("QSQLITE", dbId);
        m_db.setDatabaseName(this->m_dbPath);
        bool hasOpened = m_db.open();

//        qDebug() << "m_db.databaseName =" << m_db.databaseName();

//        qDebug() << "hasOpened" << hasOpened;

        if (hasOpened) {
            connectionCount++;
        }

        return hasOpened;
    }
    else {
        QStringList names = QSqlDatabase::connectionNames();
        m_db = QSqlDatabase::cloneDatabase(QSqlDatabase::database("qt_sql_default_connection"), "SQLITE");
        m_db.open();
//        m_db.setDatabaseName(this->m_dbPath);
//        qDebug() << "m_db.databaseName =" << m_db.databaseName();

//        qDebug() << "hasOpened" << m_db.isOpen();

        bool wasOpen = m_db.isOpen();

        return wasOpen;
    }


    return true;

}

bool DBManager::disconnect() {
    if (m_db.isOpen()) {
        m_db.close();
//        qDebug() << "Database isOpen" << m_db.isOpen();
    }

    if (QSqlDatabase::contains("sql_" + QString::number(connectionCount))) {
        QSqlDatabase::removeDatabase("sql_" + QString::number(connectionCount));
    }
//    qDebug() << "current database " << this->m_db.databaseName() << "connection name" << this->m_db.connectionName();
//    m_db.removeDatabase("QSQLITE");

    return true;
}


int DBManager::queryNumRowsForPlaylist(int playlistId)  {
    this->connect();


    QSqlQuery query(this->m_db);
    query.prepare("select count(*) as num_rows from playlist_songs where playlist_id = 0");
//    query.bindValue(":playlist_id", playlistId);

    if (! query.exec()) {
        qWarning() << "Something went wrong with counting songs from a playlist";
    }
    else {
        query.seek(0);

        return query.record().value(0).toInt();
//        qDebug() << "num_records=" << value;
//        qDebug() << getLastExecutedQuery(query);
    }

    this->disconnect();

    return 0;
}

QSqlRecord DBManager::getRecordAt(int rowId, int playlistId) {
    QSqlQuery query(this->m_db);
    query.prepare("select * from playlist_songs where playlist_id = :playlist_id and rowid = :row_id");
    query.bindValue(":playlist_id", playlistId);
    query.bindValue(":row_id", rowId);

    if (! query.exec()) {
        qWarning() << "Something went wrong with counting songs from a playlist";
        qWarning() << getLastExecutedQuery(query);
    }
    else {
        query.seek(0);

    }
    QSqlRecord record = query.record();

//    qDebug() << rowId << record.value("song_name").toString() << record.value(0).toString();

    return query.record();

}

// Todo: Separate in a DBManager base class
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
