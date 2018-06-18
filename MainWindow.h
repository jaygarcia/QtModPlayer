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
public:

    DBManager *m_dbManager;

    MainWindow(QWidget *parent = 0);
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
    void onModPositionChanged(int order, int pattern, int row);
};

#endif // MAINWINDOW_H
