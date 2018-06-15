#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QDebug>
#include <libopenmpt/libopenmpt_ext.hpp>
#include <fstream>
#include <QJsonObject>
#include <QtCore>
#include <QThread>

class SoundManager : public QObject {
    Q_OBJECT

private:
    openmpt::module_ext *m_modFile = nullptr;
    QMutex *m_mutex;


    bool isLoaded = false;
    int m_playMode = 0; // 0 stopped, 1 playing, 2 paused;
//    openmpt_module *m_modFile = nullptr;

public:
    explicit SoundManager(QObject *parent = nullptr);

    void run(); // For the thread

    void pause();
    void play();
    void stop();
    bool loadFile(QJsonObject *fileObject);
};

#endif // SOUNDMANAGER_H
