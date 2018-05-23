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

    connect(thread, &QThread::started, dbManager, &DBManager::run);

    connect(dbManager, &DBManager::insertPercentUpdate, this, [this](int pctComplete) {
       qDebug() << "Insert % complete ::" << pctComplete;
       // Todo : Emit file insert percentage so we can update the Playlist progress bar
    });


    connect(dbManager, &DBManager::insertComplete, this, [this](int totalDone) {
        qDebug() << "Insert DONE ::" << totalDone;
        // todo : Emit done
    });

    thread->start();

}


void ThreadedModFileInserter::onFileInsert() {

}
