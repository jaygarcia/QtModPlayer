#include "MainWindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QDir homeDir = QDir::home();


    if (! homeDir.exists(".QTModPlayer") && ! homeDir.mkdir(".QtModPlayer")) {
        qWarning() << "Cannot make data directory!";
    }

    if(globalStateObject->getState("repeatState").isNull()) {
      globalStateObject->setState("repeatState", false);
    }

    m_dbManager = new DBManager();
    m_dbManager->checkForDeployedDatabase();

    m_playerWidget = new PlayerWidget(this);
    this->setCentralWidget(m_playerWidget);

    connect(m_playerWidget, &PlayerWidget::showPlaylist, this, &MainWindow::onPlayerWidgetShowPlayList);


    connect(m_playerWidget->m_playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonPress);
    connect(m_playerWidget->m_pauseButton, &QPushButton::clicked, this, &MainWindow::onPauseButtonPress);

//    connect(m_playerWidget, &PlayerWidget::stop, this, &MainWindow::onStopButtonPress);

    connect(m_playerWidget->m_songPositionSlider, &QSlider::valueChanged, this, &MainWindow::onSongPositionSliderChange);

    connect(m_playerWidget->m_nextTrackButton, &QPushButton::clicked, this, &MainWindow::onNextTrackButtonPress);

    connect(m_playerWidget->m_previousTrackButton, &QPushButton::clicked, this, &MainWindow::onPreviousTrackButtonPress);

    connect(m_playerWidget->m_randomButton, &QPushButton::clicked, this, &MainWindow::onRandomButtonPress);

    connect(m_playerWidget->m_repeatButton, &QPushButton::clicked, this, &MainWindow::onRepeatButtonPress);

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

        if (globalStateObject->getState("playlistWidgetShowing").toBool() == true) {
            this->getPlaylist()->dropEvent(e);
        }
    }
}

void MainWindow::onPlayerWidgetShowPlayList() {
    this->togglePlaylistWindow();
}

void MainWindow::togglePlaylistWindow() {
    if (globalStateObject->getState("playlistWidgetShowing").toBool() == false) {
        this->showPlaylistWindow();
    }
    else if (globalStateObject->getState("playlistWidgetShowing").toBool() == true) {
        this->hidePlaylistWindow();
    }
}

void MainWindow::showPlaylistWindow() {
    if (globalStateObject->getState("playlistWidgetShowing").toBool() == false) {

        PlaylistWidget *playlist = new PlaylistWidget();
        playlist->setObjectName("playlist");
        playlist->setAttribute(Qt::WA_DeleteOnClose);
        playlist->setFixedWidth(this->geometry().width() * 2);
        playlist->setMinimumHeight(300);
        playlist->setMaximumHeight(500);
        playlist->setStyleSheet("margin : 0");

        playlist->move(this->pos().x() - (playlist->geometry().width() / 4), this->pos().y() + this->geometry().height() + 23);
        playlist->show();


        globalStateObject->setState("playlistWidgetShowing", true);
        this->m_playlistWindow = playlist;

        // Todo: visit these and see if we should use lambdas or not.
        connect(playlist, &PlaylistWidget::destroyed, this, [this](QObject *) {
            globalStateObject->setState("playlistWidgetShowing", false);
            this->m_playlistWindow = nullptr;
        });

        connect(playlist, &PlaylistWidget::playlistSelected, this, [this](QString playlistTable) {
//            globalStateObject->value("selectedTableName").toString() = playlistTable;
            Q_UNUSED(playlistTable);
//            globalStateObject->value("selectedTableName");
//            if (globalStateObject->getState("randomState").toBool()) {
            m_randomPlaylistStack.clear();
            m_randomPlaylistStackPosition = 0;
//            }
            if (m_soundManager) {
//                this->m_soundManager->stop();
//                this->m_soundManager->thread()->quit();
//                this->m_soundManager->thread()->wait();
//                this->m_soundManager = nullptr;
            }
        });

        connect(playlist, &PlaylistWidget::songSelectionChange, this, &MainWindow::onSongSelectionChange);


        // Todo: figure out a way to get this to a global state store.
//        connect(playlist, &PlaylistWidget::destroyed, this, [this](QObject *widgetObject) {
//            PlaylistWidget *playlist = (PlaylistWidget *) widgetObject;

////            QJsonObject *playlistUiState = playlist->getUiState();

////            qDebug() << "playlistUiState->keys == " << playlistUiState->keys();

//        });
    }
}

void MainWindow::hidePlaylistWindow() {
    if (globalStateObject->getState("playlistWidgetShowing").toBool() == true) {
        this->getPlaylist()->hide();
        globalStateObject->setState("playlistWidgetShowing", false);
    }
}

void MainWindow::onInserterPercentUpdate(int pctComplete) {
  Q_UNUSED(pctComplete);
//    qDebug() << "Insertion %: " << pctComplete;
    if (globalStateObject->getState("playlistWidgetShowing").toBool() == true) {
        this->getPlaylist()->refreshTableView();
    }
}

void MainWindow::onInserterComplete(int totalFiles) {
    qDebug() << "Insertion complete: " << totalFiles << "Files";
    if (globalStateObject->getState("playlistWidgetShowing").toBool() == true) {
        this->getPlaylist()->refreshTableView();
    }
}
void MainWindow::onModPositionChanged(QJsonObject *modInfoObject) {
    m_playerWidget->updateSongInformation(modInfoObject);
}

void MainWindow::onSongSelectionChange(QJsonObject *fileObject) {
//    qDebug() << Q_FUNC_INFO << fileObject->value("rowid").toInt();

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
    connect(thread, &QThread::started,  m_soundManager, &SoundManager::run);
    connect(thread, &QThread::finished, m_soundManager, &QObject::deleteLater);

    connect(m_soundManager, &SoundManager::modPositionChanged, this, &MainWindow::onModPositionChanged);

    m_soundManager->moveToThread(thread);

    m_currentModFileObject = m_soundManager->loadFile(fileObject);

    m_playerWidget->setSongPositionSliderValueSilent(0);
    m_playerWidget->m_currentOrder = 0; // Make sure the order is flushed
    m_playerWidget->updateSongInformation(m_currentModFileObject);

    thread->start();

    m_soundManager->play();
    m_playerWidget->hidePauseShowPlayButton();
}

void MainWindow::onStopButtonPress() {
  if (m_soundManager) {
      m_soundManager->stop();
  }
}

void MainWindow::onPlayButtonPress() {
    if (m_soundManager) {
        m_soundManager->play();
        m_playerWidget->hidePauseShowPlayButton();
    }
}

void MainWindow::onPauseButtonPress() {
  if (m_soundManager) {
      m_soundManager->pause();
      m_playerWidget->showPlayHidePauseButton();
  }
}

void MainWindow::onRandomButtonPress() {
  globalStateObject->setState("randomState", ! globalStateObject->getState("randomState").toBool());

  if (globalStateObject->getState("randomState").toBool()) {
      m_playerWidget->m_randomButton->setStyleSheet("background-color: #555; height: 30px; width: 30px;");
  }
  else {
      m_randomPlaylistStack.clear();
      m_randomPlaylistStackPosition = -1;
      m_playerWidget->m_randomButton->setStyleSheet("height: 30px; width: 30px;");
  }
}

void MainWindow::onRepeatButtonPress() {
  globalStateObject->setState("repeatState", ! globalStateObject->getState("repeatState").toBool());

  if (globalStateObject->getState("repeatState").toBool() == true) {
      m_playerWidget->m_repeatButton->setStyleSheet("background-color: #555; height: 30px; width: 30px;");
  }
  else {
      m_playerWidget->m_repeatButton->setStyleSheet("height: 30px; width: 30px;");
  }
}

void MainWindow::onNextTrackButtonPress() {
  if (globalStateObject->value("selectedTableName").toString().isEmpty()) {
      qWarning() << Q_FUNC_INFO << "globalStateObject->value(\"selectedTableName\") is empty!";
      return;
  }

  int songCount = m_dbManager->getSongCountFromPlaylist(globalStateObject->value("selectedTableName").toString());

  int newIndex = 0;

  int currentIndex = (m_currentModFileObject) ? this->m_currentModFileObject->value("rowid").toInt() : 0;

  QJsonObject *newModFile;
  // Sequential song selection handling, forward direction
  if (globalStateObject->getState("randomState") == false) {
//            qDebug() << "********** SEQUENTIAL forward!!";
      if (songCount > currentIndex) {
          // we use currentIndex because rowId is always 1 value ahead of the tableview.
          newIndex = currentIndex + 1;
      }

      newModFile = this->m_dbManager->getRecordAt(newIndex, globalStateObject->value("selectedTableName").toString());
  }
  //  Randomized
  else {
      newModFile = this->m_dbManager->getRecordAt(-1, globalStateObject->value("selectedTableName").toString());
      newIndex = newModFile->value("rowid").toInt();

      if (m_randomPlaylistStackPosition < m_randomPlaylistStack.count() - 1) {
          // Forward one in the stack
          newIndex = m_randomPlaylistStack.at(++m_randomPlaylistStackPosition);
      }
      else {

          m_randomPlaylistStackPosition = m_randomPlaylistStack.count();
          // Push to the stack
          m_randomPlaylistStack.push_back(newIndex);
      }

  }

  if (m_playlistWindow) {
      // Normal workflow
      QTableView *tableView = m_playlistWindow->m_tableView;
      if (newIndex > 0) {
          newIndex--;
      }
      tableView->setCurrentIndex(tableView->model()->index(newIndex, 0));
  }
  else if (newModFile) {
      // Force a song load
      this->onSongSelectionChange(newModFile);
      // Todo: update tableview state forceably

      UiStateObject *tableViewState = new UiStateObject("PlaylistWidget");
      tableViewState->setState("selectedRowIndex", newIndex);
      tableViewState->setState("selectedRowObject", QJsonObject::fromVariantMap(newModFile->toVariantMap()));
  }
}


void MainWindow::onPreviousTrackButtonPress() {
  if (globalStateObject->value("selectedTableName").toString().isEmpty()) {
      qWarning() << Q_FUNC_INFO << "globalStateObject->value(\"selectedTableName\") is empty!";
      return;
  }

  int songCount = m_dbManager->getSongCountFromPlaylist(globalStateObject->value("selectedTableName").toString());

  int newIndex = 0;

  int currentIndex = (m_currentModFileObject) ? this->m_currentModFileObject->value("rowid").toInt() : 0;

  QJsonObject *newModFile;
  // Sequential song selection handling, forward direction
  if (globalStateObject->getState("randomState") == false) {
//            qDebug() << "********** SEQUENTIAL backward!!";
      if (currentIndex > 1) {
          // we use currentIndex because rowId is always 1 value ahead of the tableview.
          newIndex = currentIndex - 1;
      }
      else {
          newIndex = songCount;
      }

      newModFile = this->m_dbManager->getRecordAt(newIndex, globalStateObject->value("selectedTableName").toString());
  }
  //  Randomized
  else {
      if (m_randomPlaylistStack.isEmpty() || m_randomPlaylistStackPosition <= 0) {
          newModFile = this->m_dbManager->getRecordAt(-1, globalStateObject->value("selectedTableName").toString());
          newIndex = newModFile->value("rowid").toInt();

          m_randomPlaylistStack.push_front(newIndex);
          m_randomPlaylistStackPosition = 0;
      }
      else {
          newModFile = this->m_dbManager->getRecordAt(m_randomPlaylistStack.at(--m_randomPlaylistStackPosition), globalStateObject->value("selectedTableName").toString());
          newIndex = newModFile->value("rowid").toInt();
      }



  }

  if (m_playlistWindow) {
      // Normal workflow
      QTableView *tableView = m_playlistWindow->m_tableView;
      if (newIndex > 0) {
          newIndex--;
      }
      tableView->setCurrentIndex(tableView->model()->index(newIndex, 0));
  }
  else if (newModFile) {
      // Force a song load
      this->onSongSelectionChange(newModFile);

      // Force state
      UiStateObject *tableViewState = new UiStateObject("PlaylistWidget");
      tableViewState->setState("selectedRowIndex", newIndex);
      tableViewState->setState("selectedRowObject", QJsonObject::fromVariantMap(newModFile->toVariantMap()));
  }
}

void MainWindow::onSongPositionSliderChange(int sliderValue) {
  m_soundManager->setModPosition(sliderValue);

  // Todo: think about moving this to the m_PlayerWidget class
  m_playerWidget->m_currentOrder = sliderValue;
  m_playerWidget->m_songStartLabel->setText(QString::number(sliderValue + 1));
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
MainWindow::~MainWindow() {

}
