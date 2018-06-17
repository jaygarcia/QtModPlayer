#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QDebug>
#include <libopenmpt/libopenmpt_ext.hpp>
#include <fstream>
#include <QJsonObject>
#include <QtCore>
#include <QThread>


#include <portaudio.h>

class SoundManager : public QObject {
    Q_OBJECT

private:


public:
    explicit SoundManager(QObject *parent = nullptr);

    void run(); // For the thread

    void pause();
    void play();
    void stop();
    QJsonObject *loadFile(QJsonObject *fileObject);
    QJsonObject *getModMetaData();

public slots:

signals:
    void modPositionChanged(int order, int pattern, int row);
};

#endif // SOUNDMANAGER_H
