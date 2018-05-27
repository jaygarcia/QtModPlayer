#include "mainwindow.h"
#include "DBManager/dbmanager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    PlayerWidget *playerWidget = new PlayerWidget(this);
    this->setCentralWidget(playerWidget);

    connect(playerWidget, &PlayerWidget::showPlaylist, this, &MainWindow::onPlayerWidgetShowPlayList);

    this->setAnimated(true);
    this->setFixedSize(300, 150);

    this->setWindowTitle("QtModPlayer");

    this->setAcceptDrops(true);

    this->m_modFileInserter = new ThreadedModFileInserter();

    connect(this->m_modFileInserter, &ThreadedModFileInserter::insertPercentUpdate, this, &MainWindow::onInserterPercentUpdate);
    connect(this->m_modFileInserter, &ThreadedModFileInserter::insertComplete, this, &MainWindow::onInserterComplete);

    this->m_playlistWidgetShowing = false;
    DBManager *dbManager = new DBManager();
    dbManager->purgeCurrentPlaylist();
}

// Todo: Move to playlist?
void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}


void MainWindow::dropEvent(QDropEvent *e) {
    if (e->mimeData()->hasUrls()) {
        this->showPlaylistWindow();

        if (this->m_playlistWidgetShowing == true) {
            this->getPlaylist()->dropEvent(e);
        }
        else {
            qDebug() << "NO PLAYLIST!";
        }
    }
}

void MainWindow::onPlayerWidgetShowPlayList() {
    this->togglePlaylistWindow();
}

void MainWindow::togglePlaylistWindow() {
    if (this->m_playlistWidgetShowing == false) {
        this->showPlaylistWindow();
    }
    else if (this->m_playlistWidgetShowing == true) {
        this->hidePlaylistWindow();
    }
}

void MainWindow::showPlaylistWindow() {
    if (this->m_playlistWidgetShowing == false) {

        PlaylistWidget *playlist = new PlaylistWidget();

        playlist->setModFileInserter(this->m_modFileInserter);
        playlist->setObjectName("playlist");
        playlist->setAttribute(Qt::WA_DeleteOnClose);
        playlist->setFixedWidth(this->geometry().width() * 2);
        playlist->setMinimumHeight(300);
        playlist->setMaximumHeight(500);

        playlist->move(this->pos().x() - (playlist->geometry().width() / 4), this->pos().y() + this->geometry().height() + 23);
        playlist->show();

        this->m_playlistWidgetShowing = true;
        this->m_playlistWindow = playlist;

        connect(playlist, &PlaylistWidget::destroyed, this, [this](QObject *) {
//            qDebug() << "playlist destroyed";
            this->m_playlistWidgetShowing = false;
        });
    }
}

void MainWindow::hidePlaylistWindow() {
    if (this->m_playlistWidgetShowing == true) {
        this->getPlaylist()->hide();
        this->m_playlistWidgetShowing = false;

    }
}

void MainWindow::onInserterPercentUpdate(int pctComplete) {
    qDebug() << "Insertion %: " << pctComplete;
    if (this->m_playlistWidgetShowing == true) {
        this->getPlaylist()->refreshTableView();
    }
}

void MainWindow::onInserterComplete(int totalFiles) {
    qDebug() << "Insertion complete: " << totalFiles << "Files";
    if (this->m_playlistWidgetShowing == true) {
        this->getPlaylist()->refreshTableView();
    }
}

PlaylistWidget *MainWindow::getPlaylist() const
{
    return m_playlistWindow;
}

void MainWindow::setPlaylist(PlaylistWidget *playlist)
{
    m_playlistWindow = playlist;
}



// Destructor
MainWindow::~MainWindow()
{

}
