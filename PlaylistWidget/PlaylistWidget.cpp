#include "PlaylistWidget.h"



PlaylistWidget::PlaylistWidget(QWidget *parent) : QWidget(parent) {
    m_uiState = new UiStateObject("PlaylistWidget");

    this->setWindowTitle("QtModPlayer :: Playlist");
    this->setAcceptDrops(true);

    this->m_progressDialog.cancel();

    QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::Down, this);
    this->setLayout(boxLayout);

    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    QHeaderView *verticalHeader = m_tableView->verticalHeader();
    verticalHeader->hide();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);

    QFont font = QFont("Helvetica", 13, QFont::ExtraLight);
    m_tableView->setFont(font);
    m_tableView->setModel(&m_model);
    m_tableView->horizontalHeader()->setStretchLastSection(true);

    this->layout()->addWidget(m_tableView);
    boxLayout->setStretch(0,1);

    m_playlistControls = new PlaylistControls(this);

    m_dbManager = new DBManager(this);
    QVector<QJsonObject *> playlists = m_dbManager->getAllPlaylists();
    this->m_playlistControls->seedComboData(playlists);

    QRect geometry = m_playlistControls->geometry();
    m_playlistControls->setGeometry(geometry);
    this->layout()->addWidget(m_playlistControls);

    QItemSelectionModel *selectionModel = m_tableView->selectionModel();

    connect(selectionModel, &QItemSelectionModel::selectionChanged, this, &PlaylistWidget::onTableViewSelectionChange);

    connect(m_playlistControls, &PlaylistControls::playlistSelectionChange, this, &PlaylistWidget::onPlaylistSelectorChange);

    connect(m_playlistControls->newPlaylistButton(), &QPushButton::clicked, this, &PlaylistWidget::onNewPlaylistButtonPress);
    connect(m_playlistControls->savePlaylistButton(), &QPushButton::clicked, this, &PlaylistWidget::onSavePlaylistButtonPress);
    connect(m_playlistControls->deletePlaylistButton(), &QPushButton::clicked, this, &PlaylistWidget::onDeletePlaylistButton);

    this->m_countingFiles = false;
}

void PlaylistWidget::loadPlaylist(QString playlistTableName) {
    this->m_playlistControls->selectPlaylistViaTableName(playlistTableName);
    m_uiState->setState("playlistTableName", playlistTableName);
}

void PlaylistWidget::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls() && ! this->m_countingFiles) {
        e->acceptProposedAction();
    }
}

void PlaylistWidget::dropEvent(QDropEvent *e) {
    QVector<QString> droppedFiles;


    if (this->m_selectedTableName.isEmpty() || this->m_selectedTableName.isNull()) {
        if (! this->getNewPlaylistNameFromUser()) {
            QMessageBox msgBox;
            msgBox.setText("A valid playlist is required to drop files.");
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.exec();

            return;
        }
    }

    this->m_countingFiles = true;

    foreach (const QUrl &url, e->mimeData()->urls()) {
        droppedFiles.push_back(url.toLocalFile());
    }

    ThreadedModFileCheck *checker = new ThreadedModFileCheck(droppedFiles);

    m_progressDialog.setParent(this);
    m_progressDialog.setMinimum(0);
    m_progressDialog.setMaximum(100);
    m_progressDialog.setWindowModality(Qt::WindowModal);
    m_progressDialog.setWindowFlag(Qt::Drawer);;
    m_progressDialog.setAutoClose(false);
    m_progressDialog.setFixedSize(this->geometry().width() - 30, m_progressDialog.geometry().height());
    m_progressDialog.open();

    QThread *thread = new QThread();
    connect(thread, &QThread::finished, checker, &QObject::deleteLater);

    checker->moveToThread(thread);

    connect(thread, &QThread::started, checker, &ThreadedModFileCheck::run);

    connect(checker, &ThreadedModFileCheck::fileCheckPercentUpdate, this, [this](int pctComplete, QJsonObject *modFile) {
        if (! this->m_progressDialog.isHidden()) {
            this->m_progressDialog.setValue(pctComplete);

            QString fileName = modFile->value("file_name").toString();
            if (!fileName.isEmpty() || !fileName.isNull()) {
                this->m_progressDialog.setLabelText(fileName);
                this->m_dbManager->addToPlaylist(this->m_selectedTableName, modFile);
//                if (pctComplete % 10 == 0) {
//                    this->m_model.refresh(this->m_selectedTableName);
//                }
            }

        }
    });

    connect(checker, &ThreadedModFileCheck::filesCounted, this, [this](unsigned int filesCounted) {
        QString friendlyNumber = QLocale(QLocale::English).toString((float)filesCounted, 'i', 0);
        this->m_progressDialog.setLabelText(QString("Validating %1 files").arg(friendlyNumber));
    });


    connect(checker, &ThreadedModFileCheck::countingFiles, this, [=](unsigned int filesCounted) {
        QString friendlyNumber = QLocale(QLocale::English).toString((float)filesCounted, 'i', 0);
        this->m_progressDialog.setLabelText(QString("Files found: %1").arg(friendlyNumber));
    });

    connect(checker, &ThreadedModFileCheck::fileCheckComplete, this, [=](ThreadedModFileCheckResults *results) {
        Q_UNUSED(results);
        this->m_progressDialog.setLabelText("...");
        this->m_progressDialog.setValue(0);
        this->m_progressDialog.hide();

        thread->quit();
        thread->wait();

        this->m_countingFiles = false;
        this->m_model.refresh(this->m_selectedTableName);
    });

    connect(&m_progressDialog, &QProgressDialog::canceled, this, [=]() {
        thread->requestInterruption();
        thread->quit();
        thread->wait();
//        this->thread()->msleep(10);
        m_progressDialog.hide();
        this->m_countingFiles = false;
    });

    thread->start();
}


void PlaylistWidget::appendFilesToModel(ThreadedModFileCheckResults *results){
    BufferedTableModel *tableModel = (BufferedTableModel*) this->m_tableView->model();

    QVector<QJsonObject *> modFiles = results->goodFiles();
    tableModel->appendItems(modFiles);
    m_playlistControls->appendFilesToModel(modFiles);
}

void PlaylistWidget::refreshTableView() {
    BufferedTableModel *model = (BufferedTableModel *)this->m_tableView->model();
    model->clearModel();
}


bool PlaylistWidget::getNewPlaylistNameFromUser() {
    bool okPressed;

    QString newPlaylistName = QInputDialog::getText(
        this,
        "New Playlist",
        "Enter a new playlist name:",
        QLineEdit::Normal,
        "",
        &okPressed
    );


    if (okPressed && !newPlaylistName.isEmpty()) {
        int newPlaylistId = this->m_dbManager->generateNewPlaylist(newPlaylistName);
        QVector<QJsonObject *> allPlaylists = this->m_dbManager->getAllPlaylists(newPlaylistId);

        this->m_playlistControls->refreshComboWithData(allPlaylists);

        QJsonObject *eventObject = new QJsonObject();
        eventObject->insert("selectedName", newPlaylistName);
        eventObject->insert("selectedTableName", "playlist_" + QString::number(newPlaylistId));
        this->onPlaylistSelectorChange(eventObject);

        return true;
    }

    return false;
}

void PlaylistWidget::onNewPlaylistButtonPress() {
    this->getNewPlaylistNameFromUser();
}

void PlaylistWidget::onTableViewSelectionChange(const QItemSelection &selected, const QItemSelection &deselected) {
    Q_UNUSED(deselected);

    qDebug() << Q_FUNC_INFO << "Selection Change " << selected;// << deselected;
    QModelIndexList list = selected.indexes();

    BufferedTableModel *tableModel = (BufferedTableModel*)list.at(0).model();
    int selectedRowIndex = list.at(0).row();

    QJsonObject *rowObject = tableModel->fetchRow(selectedRowIndex);

    m_uiState->setState("selectedRowIndex", selectedRowIndex);
    m_uiState->setState("selectedRowObject", QJsonObject::fromVariantMap(rowObject->toVariantMap()));

    emit songSelectionChange(rowObject);
}


void PlaylistWidget::onPlaylistSelectorChange(QJsonObject *selectionEvent) {
    QString selectedTableName = selectionEvent->take("tableName").toString(),
            selectedPlaylistName = selectionEvent->take("playlistName").toString();

    qDebug() << Q_FUNC_INFO << selectedTableName;
    this->m_selectedTableName = selectedTableName;
    this->m_model.refresh(selectedTableName);
    this->m_tableView->verticalScrollBar()->setSliderPosition(this->m_tableView->verticalScrollBar()->minimum());

    m_uiState->setState("selectedRowIndex", -1);
    m_uiState->setState("selectedRowObject", QJsonObject());
    m_uiState->setState("selectedTableName", selectedTableName);
    m_uiState->setState("selectedPlaylistName", selectedPlaylistName);
//    m_uiState->setState("selectedTableName", selectedPlaylistName);

    emit playlistSelected(selectedTableName);
}

void PlaylistWidget::onDeletePlaylistButton() {
    QString selectedPlaylistName = m_uiState->getState("selectedPlaylistName").toString(),
            selectedTableName = m_uiState->getState("selectedTableName").toString();

    qDebug() << Q_FUNC_INFO << selectedPlaylistName;


    QMessageBox msgBox;
    msgBox.setText("Confirm deletion of \"" + selectedPlaylistName + "\"?");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *acceptButton = msgBox.addButton("Yes", QMessageBox::AcceptRole);
    QPushButton *abortButton = msgBox.addButton(QMessageBox::Cancel);

    msgBox.exec();

    if (msgBox.clickedButton() == acceptButton) {
        qWarning() << Q_FUNC_INFO << "TODO: Execute deletion of table " + selectedTableName + " & reset selection to index 0!";
    }
    // Todo: Confirmation dialogue
    // on "YES" - Delete table (DBManager)
    //          - Clear table model
    //          - Clear selected item from drop down (maybe add item zero [empty] back in?)

//    QString selectedTableName = m_
}

// Todo: remove this method, it's prototype and the related button
void PlaylistWidget::onSavePlaylistButtonPress() {

}

