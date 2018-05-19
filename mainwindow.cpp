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
    progressDialog.hide();
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


    progressDialog.setLabelText("Checking Files");
    progressDialog.setMinimum(0);
    progressDialog.setMaximum(100);
    progressDialog.setWindowModality(Qt::WindowModal);
    progressDialog.setAutoClose(false);
//    progressDialog.setCancelButton(0); // Verify that this is the best thing!
    progressDialog.setFixedSize(progressDialog.geometry().width(), progressDialog.geometry().height());
    this->progressDialog.show();


    QThread *thread = new QThread();
    connect(thread, &QThread::finished, checker, &QObject::deleteLater);

    checker->moveToThread(thread);

    connect(thread, &QThread::started, checker, &ThreadedModFileCheck::run);

    // TODO Wire in Cancel with thread quitting https://doc.qt.io/qt-5.10/qthread.html
    connect(checker, &ThreadedModFileCheck::fileCheckPercentUpdate, this, [this](int pctComplete) {
        this->progressDialog.setValue(pctComplete);
    });

    connect(checker, &ThreadedModFileCheck::fileCheckComplete, this, [=](ThreadedModFileCheckResults *results) {
       qDebug() << "Total good files " << results->goodFileCount();
       qDebug() << "Total bad files "  << results->badFileCount();
       this->progressDialog.hide();
       thread->quit();
       thread->wait();

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


// Destructor
MainWindow::~MainWindow()
{

}
