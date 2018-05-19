#include "ThreadedModFileCheck.h"

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
    QVector<QString> goodFiles;

    int lastPctDone = 0;

    for (uint32_t i = 0; i < totalFiles; ++i) {

        if (QThread::currentThread()->isInterruptionRequested()) {
            return;
        }

        QFileInfo *fileInfo = new QFileInfo(this->allFiles.at(i));

        if (fileInfo->isFile()) {

            QString filePath = fileInfo->absoluteFilePath();
            const char *fileString = filePath.toUtf8();

            std::ifstream file(fileString, std::ios::binary);

            int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);

            if (goodLoad == openmpt::probe_file_header_result_success) {
                goodFiles.push_back(filePath);
            }
            else {
                badFiles.push_back(filePath);
            }
        }

        totalFilesChecked++;
        percentDone = (float)totalFilesChecked / (float)totalFiles;

        int pctDone = (int) (percentDone * 100.0);

        if (pctDone > lastPctDone && pctDone % 2 == 0) {
            emit fileCheckPercentUpdate(pctDone);
            lastPctDone = pctDone;
        }
    }


    ThreadedModFileCheckResults *finalResults = new ThreadedModFileCheckResults();

    finalResults->setTotalFiles(totalFiles);

    finalResults->setGoodFiles(goodFiles);
    finalResults->setGoodFileCount((int64_t) goodFiles.size());

    finalResults->setBadFiles(badFiles);
    finalResults->setBadFileCount((int64_t) badFiles.size());

    emit fileCheckComplete(finalResults);
}


// Todo: Loop through all dropped items
// -- Put them in allFiles QVector<QString> member

void ThreadedModFileCheck::queryAllDroppedItems() {
    for (int64_t i = 0; i < this->m_droppedFiles.size(); ++i) {
        QString droppedFileName = this->m_droppedFiles.at(i);

        QFileInfo *droppedFileInfo = new QFileInfo(droppedFileName);
        qDebug() << "\n*********************************\n";

        if (droppedFileInfo->isDir()) {
            qDebug() << "Dropped dir :: " << droppedFileName;
            this->searchDirectoryForFiles(droppedFileName);
        }

        if (droppedFileInfo->isFile()) {
            qDebug() << "Dropped file:" << droppedFileName;
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

QVector<QString> ThreadedModFileCheckResults::goodFiles() const
{
    return m_goodFiles;
}

void ThreadedModFileCheckResults::setGoodFiles(const QVector<QString> &goodFiles)
{
    m_goodFiles = goodFiles;
}
