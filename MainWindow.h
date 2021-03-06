#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QThread>
#include <QtCore>
#include <QCloseEvent>

#include "ThreadedModFileCheck.h"
#include "DBManager/DBManager.h"

#include "PlayerWidget.h"
#include "PlaylistWidget/PlaylistWidget.h"
#include "SoundManager.h"
#include "UiStateObject.h"

#define DEV_MODE 1


class MainWindow : public QMainWindow {
  Q_OBJECT

private:

  PlaylistWidget *m_playlistWindow;

  QString m_selectedPlaylistTable;
  SoundManager *m_soundManager = nullptr;
  PlayerWidget *m_playerWidget;

  bool m_repeatStateOn;

  QList<int> m_randomPlaylistStack;
  int m_randomPlaylistStackPosition = 0;

public:
  MainWindow(QWidget *parent = 0);

  DBManager *m_dbManager;
  QJsonObject *m_currentModFileObject;

  void dragEnterEvent(QDragEnterEvent *e);
  void dropEvent(QDropEvent *e);
  void onPlayerWidgetShowPlayList();

  void closeEvent(QCloseEvent *event);
  void togglePlaylistWindow();
  void showPlaylistWindow();
  void hidePlaylistWindow();

  ~MainWindow();

  PlaylistWidget *getPlaylist() const;
  void setPlaylist(PlaylistWidget *playlist);

  DBManager *getDbManager() const;

signals :


public slots:
  //  void onFileCountComplete(ThreadedModFileCheckResults);
  void onInserterPercentUpdate(int pctComplete);
  void onInserterComplete(int totalFiles);
  void onModPositionChanged(QJsonObject *modInfoObject);
  void onSongSelectionChange(QJsonObject *fileObject);

  void onStopButtonPress();
  void onPlayButtonPress();
  void onPauseButtonPress();
  void onRandomButtonPress();
  void onRepeatButtonPress();
  void onNextTrackButtonPress();
  void onPreviousTrackButtonPress();
  void onSongPositionSliderChange(int sliderValue);
  void onVolumeSiderChange(int sliderValue);
};

#endif // MAINWINDOW_H
