#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QThread>
#include <QtCore>

#include "ThreadedModFileCheck.h"
#include "DBManager/DBManager.h"

#include "PlayerWidget.h"
#include "PlaylistWidget/PlaylistWidget.h"
#include "SoundManager/SoundManager.h"


class MainWindow : public QMainWindow {
    Q_OBJECT

private:
    PlaylistWidget *m_playlistWindow;
    bool m_playlistWidgetShowing;
    QString m_playlistSelected;
    SoundManager *m_soundManager = nullptr;
    PlayerWidget *m_playerWidget;
    bool m_stateRandomOn;
    bool m_repeatStateOn;

public:
    MainWindow(QWidget *parent = 0);

    DBManager *m_dbManager;
    QJsonObject *m_currentModFileObject;

    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
    void onPlayerWidgetShowPlayList();


    void togglePlaylistWindow();
    void showPlaylistWindow();
    void hidePlaylistWindow();

    ~MainWindow();

    PlaylistWidget *getPlaylist() const;
    void setPlaylist(PlaylistWidget *playlist);

    DBManager *getDbManager() const;

signals :


public slots:
    //    void onFileCountComplete(ThreadedModFileCheckResults);
    void onInserterPercentUpdate(int pctComplete);
    void onInserterComplete(int totalFiles);
    void onModPositionChanged(QJsonObject *modInfoObject);
};

#endif // MAINWINDOW_H
