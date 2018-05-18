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
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString droppedFileName = url.toLocalFile();

        QFileInfo *droppedFileInfo = new QFileInfo(droppedFileName);
        qDebug() << "\n*********************************\n";

        if (droppedFileInfo->isDir()) {
            qDebug() << "Dropped dir :: " << droppedFileName;

            QThread *thread = new QThread();
            ThreadedModFileCheck *checker = new ThreadedModFileCheck();
            connect(thread, &QThread::finished, checker, &QObject::deleteLater);

            checker->m_dirName = droppedFileName;
            checker->moveToThread(thread);

            connect(thread, &QThread::started, checker, &ThreadedModFileCheck::run);

            connect(checker, &ThreadedModFileCheck::fileCheckPercentUpdate, [=](int8_t &pctComplete) {
                qDebug() << "pct complete" << pctComplete;
            });
            thread->start();
        }

        if (droppedFileInfo->isFile()) {
            qDebug() << "Dropped file:" << droppedFileName;
        }
    }
}


// Destructor
MainWindow::~MainWindow()
{

}
