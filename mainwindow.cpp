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
    songInfoWidget->setStyleSheet("background-color: #F0F");
    QHBoxLayout *songInfoLayout = new QHBoxLayout();
//    songInfoLayout->setGeometry();
    songInfoWidget->setLayout(songInfoLayout);
    songInfoLayout->setSpacing(10);

    songInfoLayout->setAlignment(Qt::AlignHCenter);

    // TODO: Make public
    int labelWidth = 35;
    QLabel *songStartLabel = new QLabel(songInfoWidget);
    songStartLabel->setText("SS:SS");
    songStartLabel->setAlignment(Qt::AlignCenter);
    songStartLabel->setFixedWidth(labelWidth);
    songInfoLayout->addWidget(songStartLabel);
    songStartLabel->setStyleSheet("background-color: blue");


    QSlider *songSlider = new QSlider(Qt::Horizontal, songInfoWidget);
    songInfoLayout->addWidget(songSlider);
    songSlider->setStyleSheet("font-size: 10px; background-color: #0FF");
//    songInfoLayout->

    // TODO: Make public
    QLabel *songEndLabel = new QLabel(songInfoWidget);
    songEndLabel->setText("EE:EE");
    songEndLabel->setAlignment(Qt::AlignCenter);
    songEndLabel->setFixedWidth(labelWidth);
    songEndLabel->setStyleSheet("font-size: 10px; background-color: yellow;");
    songInfoLayout->addWidget(songEndLabel);



    mainLayout->addWidget(songInfoWidget, 1, Qt::AlignTop);





    centralWidget->setLayout(mainLayout);

    this->setCentralWidget(centralWidget);
}

// Destructor
MainWindow::~MainWindow()
{

}
