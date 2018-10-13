#ifndef ThreadedModFileCount_H
#define ThreadedModFileCount_H
#include <QObject>
#include <QtCore>
#include <QThread>

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>

class ThreadedModFileCount : public QObject  {
  Q_OBJECT

public:
  ThreadedModFileCount(QVector<QString> droppedFiles);
  void run();

private:
  void searchDirectoryForFiles(QString dirName);
  void queryAllDroppedItems();
  QVector<QString> allFiles;
  QVector<QString> m_droppedFiles;


signals:
  void filesCounted(QJsonArray *filesCounted);
  void countingFiles(int filesCounted);
//  void fileCheckPercentUpdate(int pctComplete, QJsonObject *modFile);
//  void fileCheckComplete(ThreadedModFileCountResults* = 0); // <-- To do, find out how to pass a signal
};



#endif // ThreadedModFileCount_H
