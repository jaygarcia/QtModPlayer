#include "dbmanager.h"

DBManager::DBManager(QObject *parent) : QObject(parent)
{

}


void DBManager::addToPlaylist(QString playlistName, QVector<QString> filesToInsert) {
    qDebug() << "addToPlaylist -- " << playlistName;

    for (int i = 0; i < filesToInsert.size(); ++i) {
        qDebug() << filesToInsert.at(i);
    }
}
