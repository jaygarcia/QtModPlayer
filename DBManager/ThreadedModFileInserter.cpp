#include "ThreadedModFileInserter.h"

ThreadedModFileInserter::ThreadedModFileInserter() {
}

void ThreadedModFileInserter::run() {

}


void ThreadedModFileInserter::addToPlaylist(int playlistId, QVector<ModFile *> modFiles) {

    DBManager *dbManager = new DBManager();
    dbManager->setPlaylistId(playlistId);
    dbManager->setFilesToInsert(modFiles);

    QThread *thread = new QThread();
    connect(thread, &QThread::finished, dbManager, &QObject::deleteLater);

    dbManager->moveToThread(thread);

    connect(thread, &QThread::started, dbManager, &DBManager::bulkInsertToPlaylist);

    connect(dbManager, &DBManager::insertPercentUpdate, this, &ThreadedModFileInserter::onDbManagerInsertPercentUpdate);

    connect(dbManager, &DBManager::insertComplete, this, &ThreadedModFileInserter::onDbManagerInsertComplete);

    thread->start();
}


void ThreadedModFileInserter::onDbManagerInsertPercentUpdate(int pctDone) {
    emit insertPercentUpdate(pctDone);
}


void ThreadedModFileInserter::onDbManagerInsertComplete(int totalFiles) {
    emit insertComplete(totalFiles);
}
