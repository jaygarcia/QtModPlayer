#include "ThreadedModFileCheck.h"

// Based on: https://stackoverflow.com/a/16383433
//QString fileChecksum(const QString &fileName) {
//    QFile f(fileName);

//    QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);

//    if (f.open(QFile::ReadOnly)) {
//        if (hash.addData(&f)) {
//            return QString::fromUtf8(hash.result().toHex());
//        }
//    }

//    return QString::fromUtf8(hash.result().toHex());
//}
bool ThreadedModFileCheck::STOP_THREAD = false;
int ThreadedModFileCheck::TOTAL_FILES = -1;
int ThreadedModFileCheck::TOTAL_FILES_CHECKED = -1;
int ThreadedModFileCheck::THREADS_COMPLETED = -1;

ThreadedModFileCheck::ThreadedModFileCheck(QJsonArray *droppedFiles) {
    this->allFiles = droppedFiles;
}



void ThreadedModFileCheck::run() {

    QVector<QString> badFiles;
    QVector<QJsonObject *> goodFiles;

    while (this->allFiles->size()) {

        if (QThread::currentThread()->isInterruptionRequested() || ThreadedModFileCheck::STOP_THREAD) {
            return;
        }

        QFileInfo *fileInfo = new QFileInfo(this->allFiles->takeAt(0).toString());

        if (fileInfo->isFile()) {
            QJsonObject *checkResults = new QJsonObject();

            QString filePath = fileInfo->absoluteFilePath();
            const char *fileString = filePath.toUtf8();

            std::ifstream file(fileString, std::ios::binary);

            int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);

            if (goodLoad == openmpt::probe_file_header_result_success) {

                openmpt::module mod(file);

                QString songName = QString::fromUtf8(mod.get_metadata("title").c_str());

                checkResults->insert("file_name", fileInfo->fileName());
//                goodFile->insert("base_name", fileInfo->baseName());
                checkResults->insert("parent_directory", fileInfo->absolutePath());
                checkResults->insert("full_path", filePath);
                checkResults->insert("song_name", songName.trimmed());
                mod.~module();
            }

            file.close();
            if (ThreadedModFileCheck::STOP_THREAD == false) {
                delete fileInfo;
                emit fileChecked(checkResults);
            }

        }

    }


    QThread::msleep(500);

    emit threadComplete();


//    ThreadedModFileCheckResults *finalResults = new ThreadedModFileCheckResults();

//    finalResults->setTotalFiles(totalFiles);

//    finalResults->setGoodFiles(goodFiles);
//    finalResults->setGoodFileCount((int64_t) goodFiles.size());

//    finalResults->setBadFiles(badFiles);
//    finalResults->setBadFileCount((int64_t) badFiles.size());

//    emit fileCheckComplete(finalResults);
}

