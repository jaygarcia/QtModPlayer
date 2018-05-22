#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtCore>
#include <QtSql>
#include "../modfile.h"

class DBManager : public QObject
{
    Q_OBJECT

private:
    QSqlDatabase m_db;
    QString m_dbPath;


public:
    explicit DBManager(QObject *parent = nullptr);
    void addToPlaylist(QString playlistName, QVector<ModFile *> filesToInsert);
    bool checkForDeployedDatabase();
    bool connect();
    bool disconnect();
    void purgeCurrentPlaylist();

signals:
    void fileInserted();

public slots:
};

#endif // DBMANAGER_H
