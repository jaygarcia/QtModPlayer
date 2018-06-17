#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <QObject>
#include <QDebug>
#include <libopenmpt/libopenmpt_ext.hpp>
#include <fstream>
#include <QJsonObject>
#include <QtCore>
#include <QThread>

#include <QAudio>
#include <QAudioFormat>
#include <QAudioBuffer>
#include <QAudioOutput>

class SoundManager : public QObject {
    Q_OBJECT

private:
    openmpt::module_ext *m_modFile = nullptr;
    QMutex *m_mutex;

    QAudioFormat m_audioFormat;
    QAudioOutput *m_audioOutput;


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


public slots:
    void handleAudioStateChange(QAudio::State newState);
};

#endif // SOUNDMANAGER_H
