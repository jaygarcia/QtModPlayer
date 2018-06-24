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

    m_playerWidget = new PlayerWidget(this);
    this->setCentralWidget(m_playerWidget);

    connect(m_playerWidget, &PlayerWidget::showPlaylist, this, &MainWindow::onPlayerWidgetShowPlayList);

    connect(m_playerWidget->m_playButton, &QPushButton::clicked, this, [this]() {
//        emit pause();
        if (m_soundManager) {
            m_soundManager->play();
            m_playerWidget->hidePauseShowPlayButton();
        }
    });

    connect(m_playerWidget->m_pauseButton, &QPushButton::clicked, this, [this]() {
        if (m_soundManager) {
            m_soundManager->pause();
            m_playerWidget->showPlayHidePauseButton();
        }
    });

    connect(m_playerWidget, &PlayerWidget::stop, this, [this]() {
        if (m_soundManager) {
            m_soundManager->stop();
        }
    });

    connect(m_playerWidget->m_songPositionSlider, &QSlider::valueChanged, this, [this](int sliderValue) {
        m_soundManager->setModPosition(sliderValue);
        m_playerWidget->m_currentOrder = sliderValue;
        m_playerWidget->m_songStartLabel->setText(QString::number(sliderValue + 1));
    });


    connect(m_playerWidget->m_nextTrackButton, &QPushButton::clicked, this, [this]() {
        if (m_playlistSelected.isEmpty()) {
            return;
        }

        int songCount = m_dbManager->getSongCountFromPlaylist(m_playlistSelected);

        QTableView *tableView = m_playlistWindow->m_tableView;
        QModelIndex newIndex;

        // Sequential song selection handling, forward direction
        if (! m_stateRandomOn) {

            int currentSongIndex = (m_currentModFileObject) ? this->m_currentModFileObject->value("rowid").toInt() : 0;

            if (songCount > currentSongIndex) {
                // we use currentIndex because rowId is always 1 value ahead of the tableview.
                newIndex = tableView->model()->index(currentSongIndex, 0);
            }
            else {
                newIndex = tableView->model()->index(currentSongIndex - 1, 0);
            }
        }
        else {
            // Todo: build a "Stack model". this is completely random!
            newIndex = tableView->model()->index(rand() % songCount, 0);

            if (m_randomPlaylistStack.isEmpty()) {
                // Start from zero
                m_randomPlaylistStack.push_back(newIndex);
                m_randomPlaylistStackPosition = 0;
            }
            else if (m_randomPlaylistStackPosition < m_randomPlaylistStack.count() - 1) {
                // Forward one in the stack
                newIndex = m_randomPlaylistStack.at(++m_randomPlaylistStackPosition);
            }
            else {
                // Push to the stack
                m_randomPlaylistStack.push_back(newIndex);
                m_randomPlaylistStackPosition = m_randomPlaylistStack.count() - 1;
            }

        }

        tableView->setCurrentIndex(newIndex);
    });

    connect(m_playerWidget->m_previousTrackButton, &QPushButton::clicked, this, [this]() {
        if (m_playlistSelected.isEmpty()) {
            return;
        }

        QTableView *tableView =  m_playlistWindow->m_tableView;
        QModelIndex newIndex;

        int songCount = m_dbManager->getSongCountFromPlaylist(m_playlistSelected);
        int newSongIndex;

        // Sequential song selection handling, forward direction
        if (! m_stateRandomOn) {
            int currentSongIndex = m_currentModFileObject ? this->m_currentModFileObject->value("rowid").toInt() : 0;

            newSongIndex = (currentSongIndex == 1) ? 0 : currentSongIndex - 2;

            // we use currentIndex because rowId is always 1 value ahead of the tableview.
            newIndex = tableView->model()->index(newSongIndex, 0);
        }
        else {
            // Todo: build a "Stack model". this is completely random!
            newIndex = tableView->model()->index(rand() % songCount, 0);

            if (m_randomPlaylistStack.isEmpty()) {
                m_randomPlaylistStack.push_front(newIndex);
                m_randomPlaylistStackPosition = 0;
            }
            else if (m_randomPlaylistStackPosition > 0) {
                newIndex = m_randomPlaylistStack.at(--m_randomPlaylistStackPosition);
            }
            else if (m_randomPlaylistStackPosition <= 0){
                m_randomPlaylistStack.push_front(newIndex);
                m_randomPlaylistStackPosition = 0;

            }
        }


        tableView->setCurrentIndex(newIndex);
    });

    connect(m_playerWidget->m_randomButton, &QPushButton::clicked, this, [this]() {

        m_stateRandomOn = !m_stateRandomOn;

        if (m_stateRandomOn) {
            m_playerWidget->m_randomButton->setStyleSheet("background-color: #555; height: 30px; width: 30px;");
        }
        else {
            m_randomPlaylistStack.clear();
            m_randomPlaylistStackPosition = -1;
            m_playerWidget->m_randomButton->setStyleSheet("height: 30px; width: 30px;");
        }

    });


    connect(m_playerWidget->m_repeatButton, &QPushButton::clicked, this, [this]() {
        m_repeatStateOn = !m_repeatStateOn;

        if (m_repeatStateOn) {
            m_playerWidget->m_repeatButton->setStyleSheet("background-color: #555; height: 30px; width: 30px;");
        }
        else {
            m_playerWidget->m_repeatButton->setStyleSheet("height: 30px; width: 30px;");

        }
    });



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
            if (m_stateRandomOn) {
                m_randomPlaylistStack.clear();
                m_randomPlaylistStackPosition = 0;
            }
            if (m_soundManager) {
                this->m_soundManager->stop();
                this->m_soundManager->thread()->quit();
                this->m_soundManager->thread()->wait();
                this->m_soundManager = nullptr;
            }
        });

        connect(playlist, &PlaylistWidget::songSelectionChange, this, &MainWindow::onSongSelectionChange);
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

void MainWindow::onModPositionChanged(QJsonObject *modInfoObject) {
    m_playerWidget->updateSongInformation(modInfoObject);
}

void MainWindow::onSongSelectionChange(QJsonObject *fileObject) {
    if (this->m_soundManager == nullptr) {
        this->m_soundManager = new SoundManager();
    }
    else {
        // Todo: Reset so we can use the same soundManager
        this->m_soundManager->stop();
        this->m_soundManager->thread()->quit();
        this->m_soundManager->thread()->wait();
        this->m_soundManager = new SoundManager();
    }


    QThread *thread = new QThread();
    connect(thread, &QThread::started, m_soundManager, &SoundManager::run);
    connect(thread, &QThread::finished, m_soundManager, &QObject::deleteLater);

    connect(m_soundManager, &SoundManager::modPositionChanged, this, &MainWindow::onModPositionChanged);

    m_soundManager->moveToThread(thread);

    QJsonObject *modFileObject = m_soundManager->loadFile(fileObject);
    m_currentModFileObject = modFileObject;

    modFileObject->insert("rowid", fileObject->value("rowid").toInt());

    m_playerWidget->setSongPositionSliderValueSilent(0);
    m_playerWidget->m_currentOrder = 0; // Make sure the order is flushed
    m_playerWidget->updateSongInformation(modFileObject);

    thread->start();

    m_soundManager->play();
    m_playerWidget->hidePauseShowPlayButton();
}


// Destructor
MainWindow::~MainWindow()
{

}
