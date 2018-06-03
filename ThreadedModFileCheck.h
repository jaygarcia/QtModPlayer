#ifndef THREADEDMODFILECHECK_H
#define THREADEDMODFILECHECK_H
#include <QObject>
#include <QtCore>
#include <QThread>

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>


class ThreadedModFileCheckResults : public QObject {
    Q_OBJECT


public :

    QVector<QString>badFiles() const;
    void setBadFiles(const QVector<QString> &badFiles);

    int64_t totalFiles() const;
    void setTotalFiles(const int64_t &totalFiles);

    int64_t goodFileCount() const;
    void setGoodFileCount(const int64_t &goodFileCount);

    int64_t badFileCount() const;
    void setBadFileCount(const int64_t &badFileCount);


    QVector<QJsonObject *> goodFiles() const;
    void setGoodFiles(const QVector<QJsonObject *> &goodFiles);

private :
    QVector<QJsonObject *> m_goodFiles;
    QVector<QString> m_badFiles;
    int64_t m_totalFiles;
    int64_t m_goodFileCount;
    int64_t m_badFileCount;
};


class ThreadedModFileCheck : public QObject
{
    Q_OBJECT

public:
    ThreadedModFileCheck(QVector<QString> droppedFiles);
    void run();

private:
    void searchDirectoryForFiles(QString dirName);
    void queryAllDroppedItems();
    QVector<QString> allFiles;
    QVector<QString> m_droppedFiles;


signals:
    void filesCounted(unsigned int filesCounted);
    void countingFiles(unsigned int filesCounted);
    void fileCheckPercentUpdate(int pctComplete, QString baseName);
    void fileCheckComplete(ThreadedModFileCheckResults* = 0); // <-- To do, find out how to pass a signal
};



#endif // THREADEDMODFILECHECK_H
