#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->configure();
    this->addChildren();
}


void MainWindow::configure() {
    this->setAnimated(true);
    this->setFixedSize(300, 150);

    this->setWindowTitle("QtModPlayer");
}


//https://github.com/gamecreature/QtAwesome
void MainWindow::addChildren() {
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setGeometry(this->geometry());


    songLabel = new QLabel(centralWidget);
    songLabel->setText("0BiT - Fax Spider 2.1 kg.xm");
    songLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    songLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(songLabel);

    /** Song information Layout **/
    QWidget *songInfoWidget = new QWidget(this);
    QHBoxLayout *songInfoLayout = new QHBoxLayout();
    songInfoWidget->setLayout(songInfoLayout);

//    songInfoWidget->setStyleSheet("background-color: red");

    songInfoLayout->setAlignment(Qt::AlignCenter);

    // TODO: Make public
    QLabel *songStartLabel = new QLabel(songInfoWidget);
    songStartLabel->setText("SS:SS");
    songStartLabel->setAlignment(Qt::AlignCenter);
    songStartLabel->setFixedWidth(30);
    songInfoLayout->addWidget(songStartLabel, 0, Qt::AlignCenter);

    QSlider *songSlider = new QSlider(Qt::Horizontal, songInfoWidget);
    songInfoLayout->addWidget(songSlider, 2, Qt::AlignCenter);
//    songInfoLayout->

    // TODO: Make public
    QLabel *songEndLabel = new QLabel(songInfoWidget);
    songEndLabel->setText("EE:EE");
    songEndLabel->setAlignment(Qt::AlignCenter);
    songInfoLayout->addWidget(songEndLabel, 0, Qt::AlignCenter);



    mainLayout->addWidget(songInfoWidget, 1, Qt::AlignTop);





    centralWidget->setLayout(mainLayout);

    this->setCentralWidget(centralWidget);
}

// Destructor
MainWindow::~MainWindow()
{

}
