#include "PlayerWidget.h"

PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent)
{
    m_qtAwesome = new QtAwesome(qApp);
    m_qtAwesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map
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

    m_songLabel = new QLabel(this);
    m_songLabel->setText("");
    m_songLabel->setStyleSheet("font-size: 12px;");
//    m_songLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    m_songLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_songLabel);

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
    m_songStartLabel = new QLabel(songInfoWidget);
    m_songStartLabel->setText("0");
    m_songStartLabel->setAlignment(Qt::AlignRight);
    m_songStartLabel->setFixedWidth(labelWidth);
    m_songStartLabel->setStyleSheet("font-size: 10px;");
    songInfoLayout->addWidget(m_songStartLabel);


    QSlider *songSlider = new QSlider(Qt::Horizontal, songInfoWidget);
    songSlider->setStyleSheet("QSlider {height: 5px}");
//    songSlider->setStyleSheet("QSlider::handle:horizontal {width: 5px;}");
    songInfoLayout->addWidget(songSlider);

    // TODO: Make public
    m_songEndLabel = new QLabel(songInfoWidget);
    m_songEndLabel->setText("0");
    m_songEndLabel->setAlignment(Qt::AlignLeft);
    m_songEndLabel->setFixedWidth(labelWidth);
    m_songEndLabel->setStyleSheet("font-size: 10px;");
    songInfoLayout->addWidget(m_songEndLabel);

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
    connect(playButton, &QPushButton::clicked, this, [this]() {
       emit play();
    });

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

    QPushButton *button = new QPushButton(this->m_qtAwesome->icon(iconType, options), "");

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

    QPushButton *volumeLow = new QPushButton(this->m_qtAwesome->icon("volumedown", options), "");
    volumeLow->setStyleSheet("border-radius:99px");
    widget->layout()->addWidget(volumeLow);

    QSlider *volumeSlider = new QSlider(Qt::Horizontal);
    widget->layout()->addWidget(volumeSlider);

    QPushButton *volumeHigh = new QPushButton(this->m_qtAwesome->icon("volumeup", options), "");
    volumeHigh->setStyleSheet("border-radius:99px");
    widget->layout()->addWidget(volumeHigh);

    return widget;
}

void PlayerWidget::setSongText(QString songText) {

    m_songLabel->setText(songText);
}
