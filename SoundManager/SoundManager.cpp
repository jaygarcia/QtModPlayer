#include "SoundManager.h"

SoundManager::SoundManager(QObject *parent) : QObject(parent) {
    m_mutex = new QMutex();

    QAudioFormat format;
    m_audioFormat = format;


    m_audioFormat.setSampleRate(44100);
    m_audioFormat.setChannelCount(2);
    m_audioFormat.setCodec("audio/pcm");

    m_audioOutput = new QAudioOutput(m_audioFormat, this);

    connect(m_audioOutput, &QAudioOutput::stateChanged, this, &SoundManager::handleAudioStateChange);

//    qDebug() << Q_FUNC_INFO << m_mutex;
}


void SoundManager::run() {
    qDebug() << Q_FUNC_INFO << "Start of run!()";
    while (this->m_playMode > 0 && ! QThread::currentThread()->isInterruptionRequested()) {
        qDebug() << Q_FUNC_INFO << "running..." << m_modFile;
        this->thread()->msleep(500);
    }

    m_audioOutput->stop();
    qDebug() << Q_FUNC_INFO << "End of run!()";
}


bool SoundManager::loadFile(QJsonObject *fileObject) {
    this->stop();

    m_mutex->lock();
    bool retValue = false;

    QString filePath =  fileObject->value("full_path").toString();
    const char *fileString = filePath.toUtf8();

    std::ifstream file(fileString, std::ios::binary);

    int goodLoad = openmpt::probe_file_header(openmpt::probe_file_header_flags_default, file);

    if (goodLoad == openmpt::probe_file_header_result_success) {

        m_modFile = new openmpt::module_ext(file);

        QString songName = QString::fromUtf8(m_modFile->get_metadata("title").c_str());

        qDebug() << "Loaded song " << songName << "from file" << filePath;

        retValue = true;
    }
    m_mutex->unlock();


    this->play();
    return retValue;
}


void SoundManager::pause() {
    qDebug() << Q_FUNC_INFO;
    m_mutex->lock();
    this->m_playMode = 2;

    m_mutex->unlock();
}

void SoundManager::play() {
    qDebug() << Q_FUNC_INFO;
    m_mutex->lock();
    this->m_playMode = 1;

    m_mutex->unlock();
}

void SoundManager::stop() {
    qDebug() << Q_FUNC_INFO;

    m_mutex->lock();
    this->m_playMode = 0;

    m_mutex->unlock();
}

void SoundManager::handleAudioStateChange(QAudio::State newState) {
    qDebug() << Q_FUNC_INFO;
    switch (newState) {
       case QAudio::IdleState:
           // Finished playing (no more data)
           m_audioOutput->stop();
           break;

       case QAudio::StoppedState:
           if (m_audioOutput->error() != QAudio::NoError) {
               // Error handling
               qDebug() << "Error w/ audio";
           }
           break;

       default:
           // ... other cases as appropriate
           break;
   }
}
