#include "PlaylistWidget.h"
#include "ThreadedModFileCounter.h"


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


    // Restore list & selected item
    QString selectedTableName = globalStateObject->getState("selectedTableName").toString();
    if (! selectedTableName.isNull() && ! selectedTableName.isEmpty()) {
        this->m_model.refresh(selectedTableName);
    }

    QItemSelectionModel *selectionModel = m_tableView->selectionModel();
    if (! globalStateObject->getState("selectedRowIndex").isNull()) {
//        qDebug() << "selectedRowIndex" < globalStateObject->getState("selectedRowIndex").toInt();
        selectionModel->selectedRows(globalStateObject->getState("selectedRowIndex").toInt());
        m_tableView->selectRow(globalStateObject->getState("selectedRowIndex").toInt());
    }

    m_playlistControls = new PlaylistControls(this);

    m_dbManager = new DBManager(this);
    QVector<QJsonObject *> playlists = m_dbManager->getAllPlaylists();
    this->m_playlistControls->seedComboData(playlists);

    QRect geometry = m_playlistControls->geometry();
    m_playlistControls->setGeometry(geometry);
    this->layout()->addWidget(m_playlistControls);


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



    m_progressDialog.setParent(this);
    m_progressDialog.setMinimum(0);
    m_progressDialog.setMaximum(100);
    m_progressDialog.setWindowModality(Qt::WindowModal);
    m_progressDialog.setWindowFlag(Qt::Drawer);;
    m_progressDialog.setAutoClose(false);
    m_progressDialog.setFixedSize(this->geometry().width() - 30, m_progressDialog.geometry().height());
    m_progressDialog.open();
    m_progressDialog.setValue(0);


    ThreadedModFileCounter *counter = new ThreadedModFileCounter(droppedFiles);

    QThread *counterThread = new QThread();
    connect(counterThread, &QThread::finished, counter, &QObject::deleteLater);

    counter->moveToThread(counterThread);

    connect(counterThread, &QThread::started, counter, &ThreadedModFileCounter::run);



    connect(counter, &ThreadedModFileCounter::countingFiles, this, [=](int filesCounted) {
        QString friendlyNumber = QLocale(QLocale::English).toString((float)filesCounted, 'i', 0);
        this->m_progressDialog.setLabelText(QString("Files found: %1").arg(friendlyNumber));
    });

    int totalThreads = QThread::idealThreadCount();


    connect(counter, &ThreadedModFileCounter::filesCounted, this, [=, this](QJsonArray *allFiles) {

        ThreadedModFileCheck::TOTAL_FILES = allFiles->size();
        ThreadedModFileCheck::TOTAL_FILES_CHECKED = 0;
        ThreadedModFileCheck::THREADS_COMPLETED = 0;
        ThreadedModFileCheck::STOP_THREAD = false;

        for (int threadNum = 0; threadNum < totalThreads; threadNum++) {

            ThreadedModFileCheck *fileChecker = new ThreadedModFileCheck(allFiles);

            QThread *checkerThread = new QThread();

            connect(checkerThread, &QThread::finished, fileChecker, &QObject::deleteLater);

            fileChecker->moveToThread(checkerThread);

            connect(checkerThread, &QThread::started, fileChecker, &ThreadedModFileCheck::run);

            connect(fileChecker, &ThreadedModFileCheck::fileChecked, this, [=](QJsonObject *fileChecked) mutable -> int {
                ThreadedModFileCheck::TOTAL_FILES_CHECKED++;
                QString fileName = fileChecked->value("file_name").toString();


                if (! fileName.isEmpty() && ThreadedModFileCheck::STOP_THREAD == false) {
                    int pctComplete = (int)(((float)ThreadedModFileCheck::TOTAL_FILES_CHECKED / (float)ThreadedModFileCheck::TOTAL_FILES) * 100.0);

                    this->m_progressDialog.setLabelText(fileName);
                    QString selectedTableName = globalStateObject->getState("selectedTableName").toString();
                    this->m_dbManager->addToPlaylist(selectedTableName, fileChecked);

                    this->m_progressDialog.setValue(pctComplete);

                    if (pctComplete % 20 == 0) {
                        this->m_model.refresh(selectedTableName);
                    }
                }
            });

            connect(fileChecker, &ThreadedModFileCheck::threadComplete, this, [=]() mutable -> int {
                ThreadedModFileCheck::THREADS_COMPLETED += 1;
//                qDebug() << "threadComplete!" << ThreadedModFileCheck::THREADS_COMPLETED;

                if ((int)ThreadedModFileCheck::THREADS_COMPLETED == (totalThreads - 1)) {
//                    qDebug() << "All done!";
                    this->m_progressDialog.setValue(100);

                    QThread::msleep(250);
                    this->m_progressDialog.hide();
                    this->m_countingFiles = false;
                }
            });

            checkerThread->start();
        }




//        QString friendlyNumber = QLocale(QLocale::English).toString((float)ThreadedModFileCheck::TOTAL_FILES, 'i', 0);
//        this->m_progressDialog.setLabelText(QString("Validating %1 files").arg(friendlyNumber));
    });



    connect(&m_progressDialog, &QProgressDialog::canceled, this, [=]() {
        ThreadedModFileCheck::STOP_THREAD = true;
        QThread::msleep(100);
        counterThread->requestInterruption();
        counterThread->quit();
        counterThread->wait();
        m_progressDialog.hide();
        this->m_countingFiles = false;
    });

    counterThread->start();
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

//    qDebug() << Q_FUNC_INFO << selectedTableName;
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

//    qDebug() << Q_FUNC_INFO << selectedPlaylistName;


    QMessageBox msgBox;
    msgBox.setText("Confirm deletion of \"" + selectedPlaylistName + "\"?");
    msgBox.setIcon(QMessageBox::Question);

    QPushButton *acceptButton = msgBox.addButton("Yes", QMessageBox::AcceptRole);

    msgBox.addButton(QMessageBox::Cancel);

    msgBox.exec();

    if (msgBox.clickedButton() == acceptButton) {

        globalStateObject->setState("selectedTableName", "");
        globalStateObject->setState("selectedTableId", -1);

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

