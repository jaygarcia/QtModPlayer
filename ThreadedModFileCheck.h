#ifndef THREADEDMODFILECHECK_H
#define THREADEDMODFILECHECK_H
#include <QObject>
#include <QtCore>
#include <QThread>

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>


class ThreadedModFileCheck : public QObject
{
    Q_OBJECT

public:
    static bool STOP_THREAD;
    static int TOTAL_FILES;
    static int TOTAL_FILES_CHECKED;
    static int THREADS_COMPLETED;
public:
    ThreadedModFileCheck(QJsonArray *droppedFiles);
    void run();
    QJsonArray *allFiles;

signals:
//    void filesCounted(unsigned int filesCounted);
//    void countingFiles(unsigned int filesCounted);
    void fileCheckPercentUpdate(int pctComplete, QJsonObject *modFile);
    void fileChecked(QJsonObject *checkResults);
    void threadComplete();
};


#endif // THREADEDMODFILECHECK_H
