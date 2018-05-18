#ifndef THREADEDMODFILECHECK_H
#define THREADEDMODFILECHECK_H
#include <QObject>
#include <QtCore>
#include <QThread>

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>

class ThreadedModFileCheckResults : public QObject {
    Q_OBJECT

private :
    QVector<QString> m_goodFiles;
    QVector<QString> m_badFiles;
    int64_t m_totalFiles;
    int64_t m_goodFileCount;
    int64_t m_badFileCount;

public :
    QVector<QString> goodFiles() const;
    void setGoodFiles(const QVector<QString> &goodFiles);

    QVector<QString>badFiles() const;
    void setBadFiles(const QVector<QString> &badFiles);

    int64_t totalFiles() const;
    void setTotalFiles(const int64_t &totalFiles);

    int64_t goodFileCount() const;
    void setGoodFileCount(const int64_t &goodFileCount);

    int64_t badFileCount() const;
    void setBadFileCount(const int64_t &badFileCount);
};


class ThreadedModFileCheck : public QObject
{
    Q_OBJECT

public:
    void run();
    QString m_dirName;

private:
    int64_t countFiles();

signals:
    void filesCounted(const int64_t &filesCounted);
    void fileCheckPercentUpdate(int8_t &);
    void fileCheckComplete(QObject* = 0); // <-- To do, find out how to pass a signal
};



#endif // THREADEDMODFILECHECK_H
