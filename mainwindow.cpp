#include "mainwindow.h"
#include "playerwidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    PlayerWidget *playerWidget = new PlayerWidget(this);
    this->setCentralWidget(playerWidget);

    this->setAnimated(true);
    this->setFixedSize(300, 150);

    this->setWindowTitle("QtModPlayer");

    this->setAcceptDrops(true);

//    ThreadedModFileCheck *checker = new ThreadedModFileCheck();
//    checker->m_dirName = "/Users/jgarcia/Music/KEYGENMUSiC MusicPack/";
//    checker->start();

//    checker->wait();
}

// Todo: Move to playlist?
void MainWindow::dragEnterEvent(QDragEnterEvent *e) {
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}


void MainWindow::dropEvent(QDropEvent *e) {

    // Todo: refactor so we can use a mixture of folders and files!

    QVector<QString> droppedFiles;
    foreach (const QUrl &url, e->mimeData()->urls()) {
        droppedFiles.push_back(url.toLocalFile());
    }


    QThread *thread = new QThread();
    ThreadedModFileCheck *checker = new ThreadedModFileCheck(droppedFiles);
    connect(thread, &QThread::finished, checker, &QObject::deleteLater);

    checker->moveToThread(thread);

    connect(thread, &QThread::started, checker, &ThreadedModFileCheck::run);

    connect(checker, &ThreadedModFileCheck::fileCheckPercentUpdate, [=](int8_t &pctComplete) {
        qDebug() << "pct complete" << pctComplete;
    });

    connect(checker, &ThreadedModFileCheck::fileCheckComplete, [=](ThreadedModFileCheckResults *results) {
       qDebug() << "Total good files " << results->goodFileCount();
       qDebug() << "Total bad files " << results->badFileCount();
    });

    thread->start();


}


// Destructor
MainWindow::~MainWindow()
{

}
