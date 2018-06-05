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

private:

public:
    ThreadedModFileInserter();
    void run();
    void addToPlaylist(int playlistId, QVector<ModFile *> modFiles);


signals:
    void insertPercentUpdate(int pctDone);
    void insertComplete(int totalFiles);

public slots:
    void onDbManagerInsertPercentUpdate(int pctDone);
    void onDbManagerInsertComplete(int totalFiles);
};



#endif // THREADEDMODFILEINSERTER_H
