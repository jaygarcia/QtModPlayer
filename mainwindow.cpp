#include "mainwindow.h"
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->configure();
    this->addChildren();
}


void MainWindow::configure() {
    this->setAnimated(true);
    this->setFixedSize(300, 150);
}


//https://github.com/gamecreature/QtAwesome
void MainWindow::addChildren() {
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *centralVBoxLayout = new QVBoxLayout;
    centralVBoxLayout->setAlignment(Qt::AlignCenter);

    QLabel *label = new QLabel(centralWidget);
    label->setText("press");
    label->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    label->setAlignment(Qt::AlignCenter);


    QRect thisGeometry = this->geometry();
    printf("Width: %i\n", thisGeometry.width());
    printf("Height: %i\n", thisGeometry.height());

    centralVBoxLayout->setGeometry(this->geometry());

    centralWidget->setLayout(centralVBoxLayout);

    this->setCentralWidget(centralWidget);
}

// Destructor
MainWindow::~MainWindow()
{

}
