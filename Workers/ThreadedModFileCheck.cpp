#include "ThreadedModFileCheck.h"


void ThreadedModFileCheck::run() {
    qDebug() << "m_dirName == " << m_dirName << "\n";
    QDirIterator *iterator = new QDirIterator(m_dirName, QDirIterator::Subdirectories);

//    ThreadedModFileCheckResults finalResults = new ThreadedModFileCheckResults();
    QObject *finalResults = new QObject();

    int64_t totalFiles = this->countFiles(),
            totalFilesChecked = 0;

    printf("Total files :: %lli\n", totalFiles); fflush(stdout);

    float percentDone;

//    finalResults.setTotalFiles(totalFiles);
//    emit filesCounted(totalFiles);

    QVector<QString> badFiles;
    QVector<QString> goodFiles;

    while (iterator->hasNext()) {

        QFileInfo *fileInfo = new QFileInfo(iterator->next());

        if (fileInfo->isFile()) {

            QString filePath = fileInfo->absoluteFilePath();
            const char *fileString = filePath.toUtf8();

//            printf("%s\n", fileString); fflush(stdout);

            std::ifstream file(fileString, std::ios::binary);

            int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);
            if (goodLoad == openmpt::probe_file_header_result_failure) {
                badFiles.push_back(filePath);
            }

            if (goodLoad == openmpt::probe_file_header_result_success) {
                goodFiles.push_back(filePath);
            }
        }
        totalFilesChecked++;
        percentDone = (float)totalFilesChecked / (float)totalFiles;


        //TODO: Get PCT Complete done
        int8_t pctDone = (int8_t) (percentDone * 100.0);
        emit fileCheckPercentUpdate(pctDone);
        printf("Percent Done :: %i | %f", pctDone, percentDone);
        fflush(stdout);
        // Todo: Connect w/ testing audio
    }

//    finalResults.setGoodFiles(goodFiles);
//    finalResults.setGoodFileCount((int64_t) goodFiles.size());

//    finalResults.setBadFiles(badFiles);
//    finalResults.setBadFileCount((int64_t) badFiles.size());

    // Emit event w/ the final results
    qDebug() << "Done checking!";
    emit fileCheckComplete(finalResults);
}

int64_t ThreadedModFileCheck::countFiles() {
    QDirIterator *iterator = new QDirIterator(m_dirName, QDirIterator::Subdirectories);

    int64_t totalFiles = 0;

    while (iterator->hasNext()) {
        iterator->next(); // pop off the stack
        totalFiles++;

    }

    return totalFiles;
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
