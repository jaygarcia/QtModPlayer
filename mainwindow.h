#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>

#include "Workers/ThreadedModFileCheck.h"
#include "DBManager/dbmanager.h"

#include "playerwidget.h"
#include "PlaylistWidget/playlistwidget.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    PlaylistWidget *m_playlistWindow;

    bool m_playlistWidgetShowing;

public:
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

public slots:
    //    void onFileCountComplete(ThreadedModFileCheckResults);
};

#endif // MAINWINDOW_H
