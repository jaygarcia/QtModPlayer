#ifndef THREADEDMODFILEINSERTER_H
#define THREADEDMODFILEINSERTER_H
#include <QObject>
#include <QtCore>
#include <QThread>

#include <libopenmpt/libopenmpt.hpp>
#include <fstream>
#include <QThread>

#include "../modfile.h"
#include "dbmanager.h"

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
    void addToPlaylist(int playlistId, QVector<ModFile *> modFiles);

private:

signals:
    void insertPercentUpdate(int pctComplete);
    void insertComplete(int totalDone);

public slots:
    void onFileInsert();
};



#endif // THREADEDMODFILEINSERTER_H
