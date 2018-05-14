#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{


    qtAwesome = new QtAwesome( qApp );
    qtAwesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map
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

    QWidget *songInfoWidget = this->buildSongInformationUI();
    mainLayout->addWidget(songInfoWidget, 1, Qt::AlignTop);

    QWidget *songControlWidget = this->buildPlayerControlUI();
    mainLayout->addWidget(songControlWidget, 1, Qt::AlignTop);

    centralWidget->setLayout(mainLayout);

    this->setCentralWidget(centralWidget);
}

QWidget *MainWindow::buildSongInformationUI() {

    /** Song information Layout **/
    QWidget *songInfoWidget = new QWidget(this);
    songInfoWidget->setStyleSheet("font-size: 10px;");
    QHBoxLayout *songInfoLayout = new QHBoxLayout();
//    songInfoLayout->setGeometry();
    songInfoWidget->setLayout(songInfoLayout);
    songInfoLayout->setSpacing(10);

    songInfoLayout->setAlignment(Qt::AlignHCenter);

    // TODO: Make public
    int labelWidth = 35;
    QLabel *songStartLabel = new QLabel(songInfoWidget);
    songStartLabel->setText("SS:SS");
    songStartLabel->setAlignment(Qt::AlignJustify);
    songStartLabel->setFixedWidth(labelWidth);
    songInfoLayout->addWidget(songStartLabel);


    QSlider *songSlider = new QSlider(Qt::Horizontal, songInfoWidget);
    songInfoLayout->addWidget(songSlider);

    // TODO: Make public
    QLabel *songEndLabel = new QLabel(songInfoWidget);
    songEndLabel->setText("EE:EE");
    songEndLabel->setAlignment(Qt::AlignCenter);
    songEndLabel->setFixedWidth(labelWidth);
    songEndLabel->setStyleSheet("font-size: 10px;");
    songInfoLayout->addWidget(songEndLabel);

    return songInfoWidget;
}

QWidget *MainWindow::buildPlayerControlUI() {
    QWidget *widget = new QWidget(this);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignCenter);
    widget->setLayout(layout);


    QPushButton *repeatButton = new QPushButton(this->qtAwesome->icon("redo"), "");
    layout->addWidget(repeatButton);

    QSpacerItem *spacer = new QSpacerItem(10, 10);
    layout->addItem(spacer);


    QPushButton *rewindButton = new QPushButton(this->qtAwesome->icon("stepbackward"), "");
    layout->addWidget(rewindButton);

    QPushButton *playButton = new QPushButton(this->qtAwesome->icon("play"), "");
    layout->addWidget(playButton);

    QPushButton *fastForwardButton = new QPushButton(this->qtAwesome->icon("stepforward"), "");
    layout->addWidget(fastForwardButton);



    widget->setStyleSheet("background-color: #AAF");
    return widget;
}

// Destructor
MainWindow::~MainWindow()
{

}
