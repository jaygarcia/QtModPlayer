#include "mainwindow.h"
#include "playerwidget.h"
#include "utilities.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    PlayerWidget *playerWidget = new PlayerWidget(this);
    this->setCentralWidget(playerWidget);

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
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString droppedFileName = url.toLocalFile();

        QFileInfo *droppedFileInfo = new QFileInfo(droppedFileName);
        qDebug() << "\n*********************************\n";

        if (droppedFileInfo->isDir()) {
            qDebug() << "Dropped dir :: " << droppedFileName;
            Utilities::traverseDirectoriesAndTestFiles(droppedFileName);
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
