#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QtCore>

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = nullptr);
    void addToPlaylist(QString playlistName, QVector<QString> filesToInsert);
signals:
    void fileInserted();

public slots:
};

#endif // DBMANAGER_H
