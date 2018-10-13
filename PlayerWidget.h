#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QObject>
#include <QWidget>
#include <QtWidgets>
#include <QtAwesome.h>
#include <QProgressDialog>

class PlayerWidget : public QWidget {
  Q_OBJECT
public:
  explicit PlayerWidget(QWidget *parent = nullptr);
  ~PlayerWidget();

  int m_currentOrder = -1;

  // Members
  QLabel *m_songLabel;
  QtAwesome *m_qtAwesome;
  QLabel *m_songStartLabel;
  QLabel *m_songEndLabel;
  QSlider *m_songPositionSlider;
  QSlider *m_volumeSlider;

  QPushButton *m_playButton;
  QPushButton *m_pauseButton;
  QPushButton *m_previousPatternButton;
  QPushButton *m_nextPatternButton;

  QPushButton *m_previousTrackButton;
  QPushButton *m_nextTrackButton;

  QPushButton *m_repeatButton;
  QPushButton *m_randomButton;
  QPushButton *m_favoriteButton;


  // Methods
  void addChildren();

  QWidget *buildSongInformationUI();
  QWidget *buildPlayerControlUI();
  QPushButton *buildButton(const char *iconType);
  QWidget *buildBottomControlUI();

  void setSongText(QString songText);
  void initializeSlider();
  void updateSongInformation(QJsonObject *modInfoObject);
  void setSongPositionSliderValueSilent(int value);

  void showPlayHidePauseButton();

  void hidePauseShowPlayButton();
  QSpacerItem *buildLargeSpacer();
  QSpacerItem *buildSmallSpacer();

signals:
  void play();
  void pause();
  void stop();

  void next();
  void previous();


//  void nextPattern();
//  void prevoiusPattern();

  void shuffle();


  void repeat();
  void showPlaylist();

public slots:
};

#endif // PLAYERWIDGET_H
