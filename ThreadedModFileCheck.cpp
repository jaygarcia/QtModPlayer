#include "ThreadedModFileCheck.h"

// Based on: https://stackoverflow.com/a/16383433
QString fileChecksum(const QString &fileName) {
    QFile f(fileName);

    QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);

    if (f.open(QFile::ReadOnly)) {
        if (hash.addData(&f)) {
            return QString::fromUtf8(hash.result().toHex());
        }
    }

    return QString::fromUtf8(hash.result().toHex());
}

ThreadedModFileCheck::ThreadedModFileCheck(QVector<QString> droppedFiles) {
    this->m_droppedFiles = droppedFiles;
}

void ThreadedModFileCheck::run() {
    this->queryAllDroppedItems();

    unsigned int totalFiles = this->allFiles.size(),
            totalFilesChecked = 0;

    float percentDone;

    emit filesCounted(totalFiles);

    QVector<QString> badFiles;
    QVector<QJsonObject *> goodFiles;

    int lastPctDone = 0;

    for (uint32_t i = 0; i < totalFiles; ++i) {

        if (QThread::currentThread()->isInterruptionRequested()) {
            return;
        }

        QFileInfo *fileInfo = new QFileInfo(this->allFiles.at(i));
        QString filePath = fileInfo->absoluteFilePath();

        if (fileInfo->isFile()) {

            const char *fileString = filePath.toUtf8();

            std::ifstream file(fileString, std::ios::binary);

            int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);

            if (goodLoad == openmpt::probe_file_header_result_success) {
                QJsonObject *goodFile = new QJsonObject();

                openmpt::module mod(file);

                QString songName = QString::fromUtf8(mod.get_metadata("title").c_str());

                goodFile->insert("file_name", fileInfo->fileName());
                goodFile->insert("file_name_short", fileInfo->baseName());
                goodFile->insert("parent_directory", fileInfo->absolutePath());
                goodFile->insert("full_path", filePath);
                goodFile->insert("song_name", songName);

                goodFiles.push_back(goodFile);

            }
            else {
                badFiles.push_back(filePath);
            }
        }

        totalFilesChecked++;
        percentDone = (float)totalFilesChecked / (float)totalFiles;

        int pctDone = (int) (percentDone * 100.0);

//        if (pctDone > lastPctDone || totalFilesChecked % 1 == 0) {
        if (! QThread::currentThread()->isInterruptionRequested() && fileInfo->isFile()) {
            emit fileCheckPercentUpdate(pctDone, QString(fileInfo->fileName()));
        }
//            lastPctDone = pctDone;
//        }

        // Give us a little time to press cancel
//        this->thread()->msleep(1);
    }


    ThreadedModFileCheckResults *finalResults = new ThreadedModFileCheckResults();

    finalResults->setTotalFiles(totalFiles);

    finalResults->setGoodFiles(goodFiles);
    finalResults->setGoodFileCount((int64_t) goodFiles.size());

    finalResults->setBadFiles(badFiles);
    finalResults->setBadFileCount((int64_t) badFiles.size());

    emit fileCheckComplete(finalResults);
}



void ThreadedModFileCheck::queryAllDroppedItems() {
    for (int64_t i = 0; i < this->m_droppedFiles.size(); ++i) {
        QString droppedFileName = this->m_droppedFiles.at(i);

        QFileInfo *droppedFileInfo = new QFileInfo(droppedFileName);

        if (droppedFileInfo->isDir()) {
            this->searchDirectoryForFiles(droppedFileName);
        }

        if (droppedFileInfo->isFile()) {
            this->allFiles.push_back(droppedFileName);
        }
    }
}


void ThreadedModFileCheck::searchDirectoryForFiles(QString dirName) {
    QDirIterator *iterator = new QDirIterator(dirName, QDirIterator::Subdirectories);

    while (iterator->hasNext() && ! QThread::currentThread()->isInterruptionRequested()) {
        this->allFiles.push_back(iterator->next()); // pop off the stack

        if (this->allFiles.size() % 15 == 0) {
            emit countingFiles(this->allFiles.size());
        }
    }
}



// ThreadedModFileCheckResults members

QVector<QString> ThreadedModFileCheckResults::badFiles() const
{
    return m_badFiles;
}

void ThreadedModFileCheckResults::setBadFiles(const QVector<QString> &badFiles)
{
    m_badFiles = badFiles;
}

int64_t ThreadedModFileCheckResults::totalFiles() const
{
    return m_totalFiles;
}

void ThreadedModFileCheckResults::setTotalFiles(const int64_t &totalFiles)
{
    m_totalFiles = totalFiles;
}

int64_t ThreadedModFileCheckResults::goodFileCount() const
{
    return m_goodFileCount;
}

void ThreadedModFileCheckResults::setGoodFileCount(const int64_t &goodFileCount)
{
    m_goodFileCount = goodFileCount;
}

int64_t ThreadedModFileCheckResults::badFileCount() const
{
    return m_badFileCount;
}

void ThreadedModFileCheckResults::setBadFileCount(const int64_t &badFileCount)
{
    m_badFileCount = badFileCount;
}

QVector<QJsonObject *> ThreadedModFileCheckResults::goodFiles() const
{
    return m_goodFiles;
}

void ThreadedModFileCheckResults::setGoodFiles(const QVector<QJsonObject *> &goodFiles)
{
    m_goodFiles = goodFiles;
}




