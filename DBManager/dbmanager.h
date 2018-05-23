#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtCore>
#include <QtSql>
#include "../modfile.h"

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>

class DBManager : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase m_db;
    QString m_dbPath;
    QVector<ModFile *> m_filesToInsert;
    int m_playlistId;


public:
    explicit DBManager(QObject *parent = nullptr);
    void addToPlaylist(int playlistId, QVector<ModFile *> filesToInsert);
    void run(); // Executed by thread
    bool checkForDeployedDatabase();
    bool connect();
    bool disconnect();
    void purgeCurrentPlaylist();

    QSqlDatabase db() const;
    void setDb(const QSqlDatabase &db);

    QString dbPath() const;
    void setDbPath(const QString &dbPath);

    QVector<ModFile *> filesToInsert() const;
    void setFilesToInsert(const QVector<ModFile *> &filesToInsert);

    int playlistId() const;
    void setPlaylistId(int playlistId);

signals:
    void fileInserted();
    void insertPercentUpdate(int pctComplete);
    void insertComplete(int totalDone);

public slots:
};

#endif // DBMANAGER_H
