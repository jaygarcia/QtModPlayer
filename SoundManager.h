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
#include "UiStateObject.h"

class SoundManager : public QObject {
  Q_OBJECT

private:

public:
  explicit SoundManager(QObject *parent = nullptr);
  ~SoundManager();

  void run(); // For the thread

  void pause();
  void play();
  void stop();
  QJsonObject *loadFile(QJsonObject *fileObject);
  QJsonObject *getModMetaData();
  void setModPosition(int position);
  void setVolume(int newVolume);
public slots:

signals:
  void modPositionChanged(QJsonObject *modInfoObject);
};

#endif // SOUNDMANAGER_H
