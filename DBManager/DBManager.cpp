#include "DBManager/DBManager.h"



QString getLastExecutedQuery(const QSqlQuery& query)
{
    QString str = query.lastQuery();
    QMapIterator<QString, QVariant> it(query.boundValues());
    while (it.hasNext()) {
        it.next();
        str.replace(it.key(), it.value().toString());
    }

    return str;
}


int connectionCount = 0;

DBManager::DBManager(QObject *parent) : QObject(parent)
{
    m_dbFileName  = "modmusic.db";

    QString destDbDir = QDir::homePath() + "/.QtModPlayer/",
            destDbFile  = destDbDir + m_dbFileName;

//    qDebug() << "DBManager destDbFile = " << destDbFile;
    this->m_dbPath = destDbFile;
}

void DBManager::purgeCurrentPlaylist() {
    this->connect();

    this->m_db.commit();

    QSqlQuery query(this->m_db);

    query.exec("drop table if exists playlist_songs_tmp; ");

    query.exec("delete from playlist_songs where playlist_id = 0; ");

    query.exec("create table playlist_songs_tmp as "
        "SELECT playlist_id, song_name, file_name, full_path, like_value, md5, in_queue "
        "FROM playlist_songs; ");

    query.exec("delete from sqlite_sequence where name = 'playlist_songs'; ");

    query.exec("insert into "
        "playlist_songs (playlist_id, song_name, file_name, full_path, like_value, md5, in_queue) "
        "SELECT playlist_id, song_name, file_name, full_path, like_value, md5, in_queue "
        "FROM playlist_songs_tmp;");

    query.exec("drop table playlist_songs_tmp;");

    query.exec("vacuum;");

    query.finish();

//    if (!query.exec()) {
//        qWarning() << "Could not purge current playlist";
//        qWarning() << query.lastError();
//        qWarning() << getLastExecutedQuery(query);
//    }

    this->m_db.commit();

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

void DBManager::addToPlaylist(QString tableName, QJsonObject *modFile) {
    if (! this->connect()) {
        qWarning() << Q_FUNC_INFO<< "Cannot connect to database!";
        return;
    }
    QSqlQuery query(this->m_db);

    query.prepare(
        "INSERT OR IGNORE INTO " + tableName + " (song_name, file_name, full_path) "
        "VALUES (:song_name, :file_name, :full_path) "
    );


    query.bindValue(":song_name", modFile->value("song_name").toString());
    query.bindValue(":file_name", modFile->value("file_name").toString());
    query.bindValue(":full_path", modFile->value("full_path").toString());

    if (query.exec()) {
        query.finish();
        this->m_db.commit();
    }
    else {
        qWarning() << Q_FUNC_INFO << modFile->value("file_name").toString() << "FAILURE!";
        qWarning() << Q_FUNC_INFO << query.lastError();
        qWarning() << Q_FUNC_INFO << getLastExecutedQuery(query);
    }

    this->disconnect();
}

// this is executed via ThreadedModFileInserter (currently not in service).
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
        "INSERT OR IGNORE INTO playlist_songs (playlist_id, song_name, file_name, full_path)"
        " VALUES (:playlist_id, :song_name, :file_name, :full_path) "
    );


    int totalSize = m_filesToInsert.size();

//    bool firstFlush = false;


    for (int i = 0; i < totalSize; ++i) {

        QJsonObject *modFile = m_filesToInsert.at(i);

        query.bindValue(":playlist_id", 0);
        query.bindValue(":file_name", modFile->value("file_name"));
        query.bindValue(":full_path", modFile->value("full_path"));

        std::ifstream file(modFile->value("full_path").toString().toUtf8(), std::ios::binary);

        openmpt::module mod(file);

        QString songName = QString::fromUtf8(mod.get_metadata("title").c_str());
        query.bindValue(":song_name", songName);

        if (query.exec()) {
            totalDone++;
            query.finish();

            this->m_db.commit();
            this->thread()->msleep(1);
        }
        else {
            qWarning() << m_filesToInsert.at(i)->value("file_name").toString() << "FAILURE!";
            qWarning() << query.lastError();
            qWarning() << getLastExecutedQuery(query);
        }

        percentDone = (float)totalDone / (float)m_filesToInsert.size();

        pctDone = (int) (percentDone * 100.0);

//        if (firstFlush == false && totalSize > 150 && i > 5) {
//            emit insertPercentUpdate(pctDone);
//            if (i > 50) {
//                firstFlush = true;
//            }
//        }
//        else {
//            if (i > 10 && pctDone > lastPctDone) {
//                emit insertPercentUpdate(pctDone);
//            }
//        }

        if (i % 5 == 0) {
            emit insertPercentUpdate(pctDone);
        }

        lastPctDone = pctDone;
    }

    emit insertComplete(totalDone);

    this->disconnect();
//    emit finished();

}

void DBManager::queueAddToPlaylist(int playlistId, QVector<QJsonObject *> filesToInsert) {
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


int DBManager::getNumRowsForPlaylist(QString tableName)  {
    this->connect();

    QSqlQuery query(this->m_db);
    QString queryString = "select count(*) as num_rows from " + tableName;
    query.prepare(queryString);

    if (! query.exec()) {
        qWarning() << "Something went wrong with counting songs from a playlist";
    }
    else {
        query.seek(0);
        return query.record().value(0).toInt();
    }

    this->disconnect();

    return 0;
}

QJsonObject *DBManager::getRecordAt(int rowId, QString tableName) {
    QSqlQuery query(this->m_db);
    QString queryString = "select rowid, song_name, file_name, full_path from " + tableName + " where rowid = :row_id";
    query.prepare(queryString);
    query.bindValue(":row_id", rowId);

    if (! query.exec()) {
        qWarning() << "Something went wrong with counting songs from a playlist";
        qWarning() << getLastExecutedQuery(query);
    }
    else {
        query.seek(0);
    }

    QSqlRecord sqlRecord = query.record();


    QJsonObject *newObject = new QJsonObject();

    newObject->insert("file_name", sqlRecord.value("file_name").toString());
    newObject->insert("song_name", sqlRecord.value("song_name").toString());
    newObject->insert("full_path", sqlRecord.value("full_path").toString());
    newObject->insert("rowid", sqlRecord.value("rowid").toInt());
    newObject->insert("table_name", tableName);

    return newObject;
}



/*  --- To be used on row removal

drop table if exists playlist_songs_tmp;

create table playlist_songs_tmp as
    SELECT
        playlist_id, song_name, file_name, full_path, like_value, md5, in_queue
    FROM
        playlist_songs;

delete from playlist_songs;
delete from sqlite_sequence where name = 'playlist_songs';

insert into
    playlist_songs
     (playlist_id, song_name, file_name, full_path, like_value, md5, in_queue)
    SELECT
        playlist_id, song_name, file_name, full_path, like_value, md5, in_queue
    FROM
        playlist_songs_tmp;

drop table playlist_songs_tmp;

vacuum;
 */


int DBManager::generateNewPlaylist(QString playlistName) {
    this->connect();
    QSqlQuery query(this->m_db);

    query.prepare("insert into playlists (playlist_name) values (:playlist_name)");
    query.bindValue(":playlist_name", playlistName);

    int lastInsertId = -1;

    if (query.exec()) {
        lastInsertId = query.lastInsertId().toInt();

        QString tableName = "playlist_" + QString::number(lastInsertId);

        query.prepare("update playlists set playlist_table_name = :table_name  where id = :playlist_id;");
        query.bindValue(":table_name", tableName);
        query.bindValue(":playlist_id", lastInsertId);

        query.exec();

        QString createPlaylistTableQuery = "create table " + tableName + " as SELECT * from playlist_prototype;";
        query.exec(createPlaylistTableQuery);
    }
    else {
        qWarning() << "Could not generate playlist" << playlistName ;
        qWarning() << query.lastError();
        qWarning() << getLastExecutedQuery(query);
    }

    this->disconnect();
    return lastInsertId;
}

QVector<QJsonObject *> DBManager::getAllPlaylists(int newlyInsertedId) {
    QVector<QJsonObject *> playlists;
    this->connect();

    QSqlQuery query(this->m_db);
    query.setForwardOnly(true);
    query.exec("select * from playlists order by playlist_name");

    while (query.next()) {
        QJsonObject *playlist = new QJsonObject();
        QSqlRecord record = query.record();
        int recordId = record.value(0).toInt();

        playlist->insert("id", recordId);
        playlist->insert("playlist_name", record.value(2).toString());
        playlist->insert("playlist_table_name", record.value(3).toString());

        if (newlyInsertedId > -1 && recordId == newlyInsertedId) {
            playlist->insert("selected", true);
        }

        playlists.push_back(playlist);
    }

    this->disconnect();
    return playlists;
}


int DBManager::getSongCountFromPlaylist(QString tableName) {
    this->connect();

    QSqlQuery query(this->m_db);

    query.prepare("select MAX(rowid) as max_rowid from " + tableName);

    if (! query.exec()) {
        qWarning() << "Something went wrong with counting songs from a playlist";
        qWarning() << getLastExecutedQuery(query);
        qWarning() << query.lastError();
    }
    else {
        query.seek(0);
    }

    this->disconnect();
    return query.record().value("max_rowid").toInt();
}


// Todo: Separate in a DBManager base class
QSqlDatabase DBManager::db() const {
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

QVector<QJsonObject *> DBManager::filesToInsert() const
{
    return m_filesToInsert;
}

void DBManager::setFilesToInsert(const QVector<QJsonObject *> &filesToInsert)
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

