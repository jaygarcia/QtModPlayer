#include "PlayerWidget.h"

PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent)
{
    qtAwesome = new QtAwesome(qApp);
    qtAwesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map
    this->configure();
    this->addChildren();
}



void PlayerWidget::configure() {
//    this->setFixedSize(300, 155);
}


void PlayerWidget::addChildren() {
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setGeometry(this->geometry());
    mainLayout->setSpacing(0);

    songLabel = new QLabel(this);
    songLabel->setText("0BiT - Fax Spider 2.1 kg.xm");
    songLabel->setStyleSheet("font-size: 12px;");
//    songLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    songLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(songLabel);

    QWidget *songInfoWidget = this->buildSongInformationUI();
    mainLayout->addWidget(songInfoWidget, 0, Qt::AlignTop);
//    songInfoWidget->setStyleSheet("background-color: #F99");

    QWidget *songControlWidget = this->buildPlayerControlUI();
    mainLayout->addWidget(songControlWidget, 1, Qt::AlignTop);
//    songControlWidget->setStyleSheet("background-color: #AFA");

    QWidget *bottomWidget = this->buildBottomControlUI();
    mainLayout->addWidget(bottomWidget);
//    bottomWidget->setStyleSheet("background-color: #AAF");

    this->setLayout(mainLayout);
}

QWidget *PlayerWidget::buildSongInformationUI() {

    /** Song information Layout **/
    QWidget *songInfoWidget = new QWidget(this);

    QHBoxLayout *songInfoLayout = new QHBoxLayout();
    songInfoWidget->setLayout(songInfoLayout);

    songInfoLayout->setAlignment(Qt::AlignCenter);

    // TODO: Make public
    int labelWidth = 35;
    QLabel *songStartLabel = new QLabel(songInfoWidget);
    songStartLabel->setText("SS:SS");
    songStartLabel->setAlignment(Qt::AlignJustify);
    songStartLabel->setFixedWidth(labelWidth);
    songStartLabel->setStyleSheet("font-size: 10px;");
    songInfoLayout->addWidget(songStartLabel);


    QSlider *songSlider = new QSlider(Qt::Horizontal, songInfoWidget);
    songSlider->setStyleSheet("QSlider {height: 5px}");
//    songSlider->setStyleSheet("QSlider::handle:horizontal {width: 5px;}");
    songInfoLayout->addWidget(songSlider);

    // TODO: Make public
    QLabel *songEndLabel = new QLabel(songInfoWidget);
    songEndLabel->setText("EE:EE");
    songEndLabel->setAlignment(Qt::AlignJustify);
    songEndLabel->setFixedWidth(labelWidth);
    songEndLabel->setStyleSheet("font-size: 10px;");
    songInfoLayout->addWidget(songEndLabel);

    return songInfoWidget;
}

QWidget *PlayerWidget::buildPlayerControlUI() {
    QWidget *widget = new QWidget(this);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    layout->setSpacing(0);
    widget->setLayout(layout);


    QPushButton *repeatButton = this->buildButton("refresh");
    layout->addWidget(repeatButton);


    QSpacerItem *spacer = new QSpacerItem(30, 10);
    layout->addItem(spacer);


    QPushButton *rewindButton = this->buildButton("backward");
    layout->addWidget(rewindButton);

    QPushButton *playButton = this->buildButton("play");
    layout->addWidget(playButton);

    QPushButton *fastForwardButton = this->buildButton("forward");
    layout->addWidget(fastForwardButton);

    layout->addItem(spacer);

    QPushButton *shuffleButton = this->buildButton("random");
    layout->addWidget(shuffleButton);

    return widget;
}

QPushButton * PlayerWidget::buildButton(const char * iconType) {
    const char * baseButtonStyle = "color: #FEFEFE; height: 30px; width: 30px; ";

    QVariantMap options;
    options.insert("color" , QColor(200,200,200));
    options.insert("color-active", QColor(200,200,200));

    QPushButton *button = new QPushButton(this->qtAwesome->icon(iconType, options), "");

    button->setStyleSheet(baseButtonStyle);
    return button;
}

QWidget *PlayerWidget::buildBottomControlUI() {
    QWidget *widget = new QWidget(this);

    widget->setLayout(new QHBoxLayout());
    widget->layout()->setAlignment(Qt::AlignHCenter);

    QPushButton *playlistButton = this->buildButton("reorder");
    widget->layout()->addWidget(playlistButton);

    connect(playlistButton, &QPushButton::clicked, this, [this]() {
        emit this->showPlaylist();
    });

    QVariantMap options;
    options.insert("color" , QColor(200,200,200));
    options.insert("color-active", QColor(200,200,200));

    QPushButton *volumeLow = new QPushButton(this->qtAwesome->icon("volumedown", options), "");
    volumeLow->setStyleSheet("border-radius:99px");
    widget->layout()->addWidget(volumeLow);

    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    widget->layout()->addWidget(volumeSlider);

    QPushButton *volumeHigh = new QPushButton(this->qtAwesome->icon("volumeup", options), "");
    volumeHigh->setStyleSheet("border-radius:99px");
    widget->layout()->addWidget(volumeHigh);

    return widget;
}
