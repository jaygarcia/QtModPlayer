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

}



// Destructor
MainWindow::~MainWindow()
{

}
