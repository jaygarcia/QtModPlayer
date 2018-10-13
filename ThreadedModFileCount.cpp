#include "ThreadedModFileCount.h"

// Based on: https://stackoverflow.com/a/16383433
//QString fileChecksum(const QString &fileName) {
//  QFile f(fileName);

//  QCryptographicHash hash(QCryptographicHash::Algorithm::Md5);

//  if (f.open(QFile::ReadOnly)) {
//    if (hash.addData(&f)) {
//      return QString::fromUtf8(hash.result().toHex());
//    }
//  }

//  return QString::fromUtf8(hash.result().toHex());
//}

ThreadedModFileCount::ThreadedModFileCount(QVector<QString> droppedFiles) {
  this->m_droppedFiles = droppedFiles;
}

void ThreadedModFileCount::run() {
//  QJsonArray *allFiles = new QJsonArray();

  for (int64_t i = 0; i < this->m_droppedFiles.size(); ++i) {
    QString droppedFileName = this->m_droppedFiles.at(i);

    QFileInfo *droppedFileInfo = new QFileInfo(droppedFileName);

    if (droppedFileInfo->isDir()) {
      this->searchDirectoryForFiles(droppedFileName);
    }

    if (droppedFileInfo->isFile()) {
//       TODO: >> DEBUG HERE. no files are going into the array :(
//      allFiles->push_back(droppedFileName);
      this->allFiles.push_back(droppedFileName);
    }
  }



  QJsonArray *allFiles = new QJsonArray();

  int size = this->allFiles.size();

  for (int i = 0; i < size; i++) {
    allFiles->append(this->allFiles.at(i));
  }

  emit filesCounted(allFiles);
}

void ThreadedModFileCount::searchDirectoryForFiles(QString dirName) {
  QDirIterator *iterator = new QDirIterator(dirName, QDirIterator::Subdirectories);

  while (iterator->hasNext() && ! QThread::currentThread()->isInterruptionRequested()) {
    QString file = iterator->next();
    QFileInfo *fileInfo = new QFileInfo(file);

    if (fileInfo->isFile()) {
      this->allFiles.push_back(file); // pop off the stack
    }

    if (this->allFiles.size() % 15 == 0) {
      emit countingFiles(this->allFiles.size());
    }
  }
}




