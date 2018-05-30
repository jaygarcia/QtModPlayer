#include "PlaylistWidget.h"



PlaylistWidget::PlaylistWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("QtModPlayer :: Playlist");

    this->setAcceptDrops(true);

    this->m_progressDialog.cancel();

    QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::Down, this);
    this->setLayout(boxLayout);

    m_tableView = new QTableView(this);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

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

    QRect geometry = m_playlistControls->geometry();
//    geometry.setHeight(30);
    m_playlistControls->setGeometry(geometry);

    this->layout()->addWidget(m_playlistControls);

    connect(m_playlistControls, &PlaylistControls::onPlaylistSelectionRefreshPlaylist, this, [](QJsonArray items) {
        qDebug() << "onPlaylistSelectorChange";

        for (int i = 0; i < items.size(); ++i) {
            qDebug() <<  items.at(i);
        }
    });

    this->m_countingFiles = false;
}



void PlaylistWidget::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls() && ! this->m_countingFiles) {
        e->acceptProposedAction();
    }
}

void PlaylistWidget::dropEvent(QDropEvent *e) {
    QVector<QString> droppedFiles;

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

    connect(checker, &ThreadedModFileCheck::fileCheckPercentUpdate, this, [this](int pctComplete) {
        if (! this->m_progressDialog.isHidden()) {
            this->m_progressDialog.setValue(pctComplete);
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
       qDebug() << "Total good files " << results->goodFileCount();
       qDebug() << "Total bad files "  << results->badFileCount();

       thread->quit();
       thread->wait();

       this->startFileInsertion(results);
       this->m_progressDialog.hide();
       this->m_countingFiles = false;
    });

    connect(&m_progressDialog, &QProgressDialog::canceled, this, [=]() {
        thread->requestInterruption();
        thread->quit();
        thread->wait();
        this->thread()->msleep(10);
        m_progressDialog.hide();
        this->m_countingFiles = false;
    });

    thread->start();
}


void PlaylistWidget::startFileInsertion(ThreadedModFileCheckResults *results){
    BufferedTableModel *model = (BufferedTableModel*) this->m_tableView->model();
    model->appendItems(results->goodFiles());

//    playlistItems
}

void PlaylistWidget::refreshTableView() {
    BufferedTableModel *model = (BufferedTableModel *)this->m_tableView->model();
    model->refresh();
}
