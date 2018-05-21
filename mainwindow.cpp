#include "mainwindow.h"
#include "playerwidget.h"
#include "playlistwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    PlayerWidget *playerWidget = new PlayerWidget(this);
    this->setCentralWidget(playerWidget);

    connect(playerWidget, &PlayerWidget::showPlaylist, this, &MainWindow::onPlayerWidgetShowPlayList);

    this->setAnimated(true);
    this->setFixedSize(300, 150);

    this->setWindowTitle("QtModPlayer");

    this->setAcceptDrops(true);

    this->progressDialog.cancel();
    this->playlistWidgetShowing = false;
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

    qDebug() << "Total items dropped" << droppedFiles.size();

    ThreadedModFileCheck *checker = new ThreadedModFileCheck(droppedFiles);

    progressDialog.setLabelText("Counting files...");
    progressDialog.setMinimum(0);
    progressDialog.setMaximum(100);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setAutoClose(false);
    progressDialog.setFixedSize(this->geometry().width(), progressDialog.geometry().height());
    progressDialog.show();


    QThread *thread = new QThread();
    connect(thread, &QThread::finished, checker, &QObject::deleteLater);

    checker->moveToThread(thread);

    connect(thread, &QThread::started, checker, &ThreadedModFileCheck::run);

    // TODO Wire in Cancel with thread quitting https://doc.qt.io/qt-5.10/qthread.html
    connect(checker, &ThreadedModFileCheck::fileCheckPercentUpdate, this, [this](int pctComplete) {
        this->progressDialog.setValue(pctComplete);
    });

    connect(checker, &ThreadedModFileCheck::filesCounted, this, [this](unsigned int filesCounted) {
        QString friendlyNumber = QLocale(QLocale::English).toString((float)filesCounted, 'i', 0);
        this->progressDialog.setLabelText(QString("Testing total files: %1").arg(friendlyNumber));
    });

    connect(checker, &ThreadedModFileCheck::fileCheckComplete, this, [=](ThreadedModFileCheckResults *results) {
       qDebug() << "Total good files " << results->goodFileCount();
       qDebug() << "Total bad files "  << results->badFileCount();
       this->progressDialog.hide();
       thread->quit();
       thread->wait();
    });

    connect(checker, &ThreadedModFileCheck::countingFiles, this, [=](unsigned int filesCounted) {

        QString friendlyNumber = QLocale(QLocale::English).toString((float)filesCounted, 'i', 0);
        this->progressDialog.setLabelText(QString("Files found: %1").arg(friendlyNumber));
    });

    connect(&progressDialog, &QProgressDialog::canceled, this, [=]() {
        thread->requestInterruption();
//        thread->wait();
        thread->quit();
        thread->wait();
        progressDialog.hide();
    });


    thread->start();

}


void MainWindow::onPlayerWidgetShowPlayList() {
    if (this->playlistWidgetShowing == false) {
        PlayListWidget *playlist = new PlayListWidget();
        playlist->setObjectName("playlist");
        playlist->setAttribute(Qt::WA_DeleteOnClose);
        playlist->setFixedSize(this->geometry().width(), 300);
        playlist->move(this->pos().x(), this->pos().y() + this->geometry().height() + 23);
        playlist->show();


        this->playlistWidgetShowing = true;

        connect(playlist, &PlayListWidget::destroyed, this, [this](QObject *) {
            this->playlistWidgetShowing = false;
        });

    }
    else if (this->playlistWidgetShowing == true) {

        QWindowList allWindows = QGuiApplication::allWindows();

        for (int i = 0; i < allWindows.size(); ++i) {
            if (allWindows.at(i)->objectName().toStdString() == "playlistWindow") {
                allWindows.at(i)->close();
                this->playlistWidgetShowing = false;
            }
        }

    }
}
// Destructor
MainWindow::~MainWindow()
{

}
