#include "PlaylistWidget.h"



PlaylistWidget::PlaylistWidget(QWidget *parent) : QWidget(parent) {

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
    globalStateObject->setState("playlistTableName", playlistTableName);
}

void PlaylistWidget::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls() && ! this->m_countingFiles) {
        e->acceptProposedAction();
    }
}

void PlaylistWidget::dropEvent(QDropEvent *e) {
    QVector<QString> droppedFiles;
//    printf("************** DROP EVENT ***********\n"); fflush(stdout);
    QString selectedTableName = globalStateObject->getState("selectedTableName").toString();
    if (selectedTableName.isEmpty() || selectedTableName.isNull()) {
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
//        printf("fileCheckPercentUpdate\n");fflush(stdout);
        if (! this->m_progressDialog.isHidden()) {
            this->m_progressDialog.setValue(pctComplete);

            QString fileName = modFile->value("file_name").toString();
            if (!fileName.isEmpty() || !fileName.isNull()) {
                this->m_progressDialog.setLabelText(fileName);
                QString selectedTableName = globalStateObject->getState("selectedTableName").toString();
                this->m_dbManager->addToPlaylist(selectedTableName, modFile);
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
//        printf("ThreadedModFileCheck::fileCheckComplete\n"); fflush(stdout);
        this->m_progressDialog.hide();

        thread->quit();
        thread->wait();

        this->m_countingFiles = false;
        this->m_model.refresh(globalStateObject->getState("selectedTableName").toString());
    });

    connect(&m_progressDialog, &QProgressDialog::canceled, this, [=]() {
        thread->requestInterruption();
        thread->quit();
        thread->wait();
        m_progressDialog.hide();
        this->m_countingFiles = false;
//        this->layout()->removeWidget(this->m_progressDialog);
    });

    thread->start();
}


void PlaylistWidget::appendFilesToModel(ThreadedModFileCheckResults *results){
    BufferedTableModel *tableModel = (BufferedTableModel *) this->m_tableView->model();

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
        int newTableId = this->m_dbManager->generateNewPlaylist(newPlaylistName);
        QVector<QJsonObject *> allPlaylists = this->m_dbManager->getAllPlaylists(newTableId);

        this->m_playlistControls->refreshComboWithData(allPlaylists);

        return true;
    }

    return false;
}

void PlaylistWidget::onNewPlaylistButtonPress() {
    this->getNewPlaylistNameFromUser();
}

void PlaylistWidget::onTableViewSelectionChange(const QItemSelection &selected, const QItemSelection &deselected) {
    Q_UNUSED(deselected);

//    qDebug() << Q_FUNC_INFO << "Selection Change " << selected;// << deselected;
    QModelIndexList list = selected.indexes();

    BufferedTableModel *tableModel = (BufferedTableModel*)list.at(0).model();
    int selectedRowIndex = list.at(0).row();

    QJsonObject *rowObject = tableModel->fetchRow(selectedRowIndex);

    globalStateObject->setState("selectedRowIndex", selectedRowIndex);
    globalStateObject->setState("selectedRowObject", QJsonObject::fromVariantMap(rowObject->toVariantMap()));

    emit songSelectionChange(rowObject);
}


void PlaylistWidget::onPlaylistSelectorChange(QJsonObject *selectionEvent) {
    QString selectedTableName = selectionEvent->value("tableName").toString(),
            selectedPlaylistName = selectionEvent->value("playlistName").toString();

    qDebug() << Q_FUNC_INFO << selectedTableName;
//    this->m_selectedTableName = selectedTableName;
    this->m_model.refresh(selectedTableName);
    this->m_tableView->verticalScrollBar()->setSliderPosition(this->m_tableView->verticalScrollBar()->minimum());

    globalStateObject->setState("selectedRowIndex", -1);
    globalStateObject->setState("selectedRowObject", QJsonObject());
    globalStateObject->setState("selectedTableName", selectedTableName);
    globalStateObject->setState("selectedPlaylistName", selectedPlaylistName);
//    qDebug() << "value(\"TableId\").toString()" << selectionEvent->value("tableId").toString();
    globalStateObject->setState("selectedTableId", selectionEvent->value("tableId").toInt());
//    globalStateObject->setState("selectedTableName", selectedPlaylistName);

    emit playlistSelected(selectedTableName);
}

void PlaylistWidget::onDeletePlaylistButton() {
    QString selectedPlaylistName = globalStateObject->getState("selectedPlaylistName").toString(),
            selectedTableName = globalStateObject->getState("selectedTableName").toString();

    int selectedTableId  = globalStateObject->getState("selectedTableId").toInt();

    qDebug() << Q_FUNC_INFO << selectedPlaylistName;


    QMessageBox msgBox;
    msgBox.setText("Confirm deletion of \"" + selectedPlaylistName + "\"?");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *acceptButton = msgBox.addButton("Yes", QMessageBox::AcceptRole);

    msgBox.addButton(QMessageBox::Cancel);

    msgBox.exec();

    if (msgBox.clickedButton() == acceptButton) {
        m_dbManager->deleteTable(selectedTableName, selectedTableId);
        refreshTableView();

        int newTableId = 0;
        QVector<QJsonObject *> allPlaylists = this->m_dbManager->getAllPlaylists(newTableId);

        this->m_playlistControls->refreshComboWithData(allPlaylists);

        QJsonObject *eventObject = new QJsonObject();
        eventObject->insert("selectedName", "<Empty Playlist>");
        eventObject->insert("selectedTableName", "playlist_" + QString::number(newTableId));
        eventObject->insert("selectedTableId", QString::number(newTableId));
        this->onPlaylistSelectorChange(eventObject);
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

