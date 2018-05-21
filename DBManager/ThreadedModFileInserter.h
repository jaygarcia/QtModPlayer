#ifndef THREADEDMODFILEINSERTER_H
#define THREADEDMODFILEINSERTER_H
#include <QObject>
#include <QtCore>
#include <QThread>

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>

class ThreadedModFileInserterResults : public QObject {
    Q_OBJECT


public :



private :

};


class ThreadedModFileInserter : public QObject
{
    Q_OBJECT

public:
    ThreadedModFileInserter();
    void run();

private:
    void searchDirectoryForFiles(QString dirName);
    void queryAllDroppedItems();


signals:
    void filesCounted(unsigned int filesCounted);
    void countingFiles(unsigned int filesCounted);
    void fileCheckPercentUpdate(int pctComplete);
    void fileCheckComplete(ThreadedModFileInserterResults* = 0); // <-- To do, find out how to pass a signal
};



#endif // THREADEDMODFILEINSERTER_H
