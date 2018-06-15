#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    QDir homeDir = QDir::home();

    if (! homeDir.exists(".QTModPlayer") && ! homeDir.mkdir(".QtModPlayer")) {
        qWarning() << "Cannot make data directory!";
    }



    m_dbManager = new DBManager();
    m_dbManager->checkForDeployedDatabase();


    PlayerWidget *playerWidget = new PlayerWidget(this);
    this->setCentralWidget(playerWidget);

    connect(playerWidget, &PlayerWidget::showPlaylist, this, &MainWindow::onPlayerWidgetShowPlayList);

    this->setAnimated(true);
    this->setFixedSize(300, 150);

    this->setWindowTitle("QtModPlayer");

    this->setAcceptDrops(true);
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
        playlist->setObjectName("playlist");
        playlist->setAttribute(Qt::WA_DeleteOnClose);
        playlist->setFixedWidth(this->geometry().width() * 2);
        playlist->setMinimumHeight(300);
        playlist->setMaximumHeight(500);
        playlist->setStyleSheet("margin : 0");

        playlist->move(this->pos().x() - (playlist->geometry().width() / 4), this->pos().y() + this->geometry().height() + 23);
        playlist->show();

        playlist->loadPlaylist(this->m_playlistSelected);


        this->m_playlistWidgetShowing = true;
        this->m_playlistWindow = playlist;

        // Todo: visit these and see if we should use lambdas or not.
        connect(playlist, &PlaylistWidget::destroyed, this, [this](QObject *) {
            this->m_playlistWidgetShowing = false;
        });

        connect(playlist, &PlaylistWidget::playlistSelected, this, [this](QString playlistTable) {
           this->m_playlistSelected = playlistTable;
        });

        connect(playlist, &PlaylistWidget::songSelectionChange, this, [this](QJsonObject *fileObject) {
            if (this->m_soundManager == 0) {
                this->m_soundManager = new SoundManager();
            }
            else {
                this->m_soundManager->stop();
                this->m_soundManager->thread()->quit();
                this->m_soundManager->thread()->wait();
                this->m_soundManager = new SoundManager();

            }

            qDebug() << "Current soundMan" << m_soundManager;

            QThread *thread = new QThread();
            connect(thread, &QThread::started, m_soundManager, &SoundManager::run);
            connect(thread, &QThread::finished, m_soundManager, &QObject::deleteLater);

            m_soundManager->moveToThread(thread);

            m_soundManager->loadFile(fileObject);

            thread->start();
//            qDebug() << Q_FUNC_INFO << "Playlist selection" << fileObject->value("full_path").toString();
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

DBManager *MainWindow::getDbManager() const
{
    return m_dbManager;
}



// Destructor
MainWindow::~MainWindow()
{

}
