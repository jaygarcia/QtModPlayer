#include "playlistwidget.h"

#include <QTableWidget>
#include <QHeaderView>

PlaylistWidget::PlaylistWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("QtModPlayer :: Playlist");

//    this->setStyleSheet("background-color: blue");
    this->setAcceptDrops(true);

    this->m_progressDialog.cancel();

    QBoxLayout *boxLayout = new QBoxLayout(QBoxLayout::Down, this);
    this->setLayout(boxLayout);



    m_tableView = new QTableView(this);

    QHeaderView *verticalHeader = m_tableView->verticalHeader();

    verticalHeader->hide();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(24);


    QFont font = QFont("Courier New", 12, QFont::Bold);
    m_tableView->setFont(font);
    m_tableView->setModel(&m_model);
    m_tableView->horizontalHeader()->setStretchLastSection(true);

    this->layout()->addWidget(m_tableView);
    boxLayout->setStretch(0,1);




//    model->setHeaderData(1, Qt::Horizontal, tr("Salary"));

//    QWidget *y = new QWidget(this);
//    y->setStyleSheet("background-color: #F0F");
//    this->layout()->addWidget(y);
//    boxLayout->setStretch(0,1);


//    QWidget *x = new QWidget(this);
//    x->setStyleSheet("background-color: #00F");
//    this->layout()->addWidget(x);
//    this->layout()->setMargin(0);

//    this->layout()->setMargin(0);
}

void PlaylistWidget::refreshTableView() {
    m_tableView->repaint();
}

void PlaylistWidget::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}


void PlaylistWidget::dropEvent(QDropEvent *e) {
    QVector<QString> droppedFiles;

    foreach (const QUrl &url, e->mimeData()->urls()) {
        droppedFiles.push_back(url.toLocalFile());
    }

    qDebug() << "Total items dropped" << droppedFiles.size();

    ThreadedModFileCheck *checker = new ThreadedModFileCheck(droppedFiles);

    m_progressDialog.setLabelText("Counting files...");
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

    // TODO Wire in Cancel with thread quitting https://doc.qt.io/qt-5.10/qthread.html
    connect(checker, &ThreadedModFileCheck::fileCheckPercentUpdate, this, [this](int pctComplete) {
        if (! this->m_progressDialog.isHidden()) {
            this->m_progressDialog.setValue(pctComplete);
        }
    });

    connect(checker, &ThreadedModFileCheck::filesCounted, this, [this](unsigned int filesCounted) {
        QString friendlyNumber = QLocale(QLocale::English).toString((float)filesCounted, 'i', 0);
        this->m_progressDialog.setLabelText(QString("Validating total files: %1").arg(friendlyNumber));
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

       qDebug() << " results.size() == " << results->goodFiles().size();
       this->startFileInsertion(results);
    });

    connect(&m_progressDialog, &QProgressDialog::canceled, this, [=]() {
        thread->requestInterruption();
        thread->quit();
        thread->wait();
        this->thread()->msleep(10);
        m_progressDialog.hide();
        m_progressDialog.hide();
    });


    thread->start();
}


void PlaylistWidget::startFileInsertion(ThreadedModFileCheckResults *results){
    this->m_progressDialog.setLabelText("Inserting...");
    this->m_progressDialog.setValue(0);


    // C++ with Lambdas seems to be very much like JavaScript!
    connect(&this->m_modFileInserter, &ThreadedModFileInserter::insertPercentUpdate, this, [this](int pctComplete) {
//        QString friendlyNumber = QLocale(QLocale::English).toString((float)pctComplete, 'i', 0);
//        this->m_progressDialog.setLabelText(QString("Inserting total files: %1").arg(friendlyNumber));
//        this->m_progressDialog.setLabelText
        this->m_progressDialog.setValue(pctComplete);
    });

    connect(&this->m_modFileInserter, &ThreadedModFileInserter::insertComplete, this, [this](int totalDone) {
        qDebug() << "Total files inserted" << totalDone;
        this->m_progressDialog.hide();
        this->m_progressDialog.hide();
        this->m_progressDialog.hide();
        AsyncBufferedTableModel *model = (AsyncBufferedTableModel *)this->m_tableView->model();
        model->refresh();
    });


    this->m_modFileInserter.addToPlaylist(0, results->goodFiles());
}
