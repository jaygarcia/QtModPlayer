#include "ThreadedModFileInserter.h"

ThreadedModFileInserter::ThreadedModFileInserter() {
}

void ThreadedModFileInserter::run() {
    this->queryAllDroppedItems();

//    unsigned int totalFiles = this->allFiles.size(),
//            totalFilesChecked = 0;

//    float percentDone;

//    emit filesCounted(totalFiles);

//    QVector<QString> badFiles;
//    QVector<QString> goodFiles;

//    int lastPctDone = 0;

//    for (uint32_t i = 0; i < totalFiles; ++i) {

////        if (pctDone > lastPctDone && pctDone % 2 == 0) {
////            emit fileCheckPercentUpdate(pctDone);
////            lastPctDone = pctDone;
////        }
//    }


    ThreadedModFileInserterResults *finalResults = new ThreadedModFileInserterResults();



    emit fileCheckComplete(finalResults);
}


// Todo: Loop through all dropped items
// -- Put them in allFiles QVector<QString> member

void ThreadedModFileInserter::queryAllDroppedItems() {
//    for (int64_t i = 0; i < this->m_droppedFiles.size(); ++i) {
//        QString droppedFileName = this->m_droppedFiles.at(i);

//        QFileInfo *droppedFileInfo = new QFileInfo(droppedFileName);
//        qDebug() << "\n*********************************\n";

//        if (droppedFileInfo->isDir()) {
//            qDebug() << "Dropped dir :: " << droppedFileName;
//            this->searchDirectoryForFiles(droppedFileName);
//        }

//        if (droppedFileInfo->isFile()) {
//            qDebug() << "Dropped file:" << droppedFileName;
//            this->allFiles.push_back(droppedFileName);
//        }
//    }
}


void ThreadedModFileInserter::searchDirectoryForFiles(QString dirName) {
//    QDirIterator *iterator = new QDirIterator(dirName, QDirIterator::Subdirectories);

//    while (iterator->hasNext() && ! QThread::currentThread()->isInterruptionRequested()) {
//        this->allFiles.push_back(iterator->next()); // pop off the stack

//        if (this->allFiles.size() % 15 == 0) {
//            emit countingFiles(this->allFiles.size());
//        }
//    }
}


