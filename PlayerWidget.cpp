#include "PlayerWidget.h"

PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent)
{
    m_qtAwesome = new QtAwesome(qApp);
    m_qtAwesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map
//    this->configure();
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
    m_songLabel->setText("(Load music)");
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


    m_songPositionSlider = new QSlider(Qt::Horizontal, songInfoWidget);
    m_songPositionSlider->setSingleStep(1);
//    m_songPositionSlider->setStyleSheet("QSlider {height: 5px}");
    m_songPositionSlider->setMinimum(0);
//    songSlider->setStyleSheet("QSlider::handle:horizontal {width: 5px;}");
    songInfoLayout->addWidget(m_songPositionSlider);

    // TODO: Make public
    m_songEndLabel = new QLabel(songInfoWidget);
    m_songEndLabel->setText("0");
    m_songEndLabel->setAlignment(Qt::AlignLeft);
    m_songEndLabel->setFixedWidth(labelWidth);
    m_songEndLabel->setStyleSheet("font-size: 10px;");
    songInfoLayout->addWidget(m_songEndLabel);

    return songInfoWidget;
}

QSpacerItem *PlayerWidget::buildLargeSpacer() {
    return new QSpacerItem(30, 10);
}
QSpacerItem *PlayerWidget::buildSmallSpacer() {
    return new QSpacerItem(5, 10);
}

QWidget *PlayerWidget::buildPlayerControlUI() {
    QWidget *widget = new QWidget(this);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignHCenter | Qt::AlignCenter);
    layout->setSpacing(0);
    widget->setLayout(layout);

    m_repeatButton = this->buildButton("refresh");
    layout->addWidget(m_repeatButton);


    layout->addItem(this->buildLargeSpacer());


    m_previousTrackButton = this->buildButton("fastbackward");
    layout->addWidget(m_previousTrackButton);
    layout->addItem(this->buildSmallSpacer());


    m_previousPatternButton = this->buildButton("backward");
    layout->addWidget(m_previousPatternButton);

    connect(m_previousPatternButton, &QPushButton::clicked, this, [this](){
       int newSliderValue = m_songPositionSlider->value() - 1;

       if (newSliderValue < m_songPositionSlider->minimum()) {
           newSliderValue = 0;
       }
       m_songPositionSlider->setValue(newSliderValue);
    });



    m_playButton = this->buildButton("play");
    layout->addWidget(m_playButton);

    m_pauseButton = this->buildButton("pause");
    layout->addWidget(m_pauseButton);
    this->showPlayHidePauseButton();

    m_nextPatternButton = this->buildButton("forward");
    layout->addWidget(m_nextPatternButton);

    connect(m_nextPatternButton, &QPushButton::clicked, this, [this](){
       int newSliderValue = m_songPositionSlider->value() + 1;

       if (newSliderValue > m_songPositionSlider->maximum()) {
           newSliderValue = m_songPositionSlider->maximum();
       }
       m_songPositionSlider->setValue(newSliderValue);
    });


    layout->addItem(this->buildSmallSpacer());

    m_nextTrackButton = this->buildButton("fastforward");
    layout->addWidget(m_nextTrackButton);

    layout->addItem(this->buildLargeSpacer());

    m_randomButton = this->buildButton("random");
    layout->addWidget(m_randomButton);

    return widget;
}

QPushButton * PlayerWidget::buildButton(const char * iconType) {
    const char * baseButtonStyle = "height: 30px; width: 30px; ";

    QVariantMap options;
    options.insert("color" , QColor(200,200,200));
    options.insert("color-active", QColor(200,200,200));

    QPushButton *button = new QPushButton(this->m_qtAwesome->icon(iconType, options), "");
    qDebug() << iconType << button->autoExclusive();
    button->setAutoExclusive(true);
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

    widget->layout()->addItem(new QSpacerItem(5, 0));
    m_favoriteButton = this->buildButton("star");
    widget->layout()->addWidget(m_favoriteButton);


    return widget;
}

void PlayerWidget::setSongText(QString songText) {
    m_songLabel->setText(songText);
}


void PlayerWidget::updateSongInformation(QJsonObject *modInfoObject) {
    QString songName;


    if (modInfoObject->value("song_name").toString().isEmpty()) {
        songName = modInfoObject->value("file_name").toString();
    }
    else {
        songName = modInfoObject->value("song_name").toString();
    }


    if (m_songLabel->text().compare(songName) != 0) {
        m_songLabel->setText(songName);
    }

    int currentOrder = modInfoObject->value("current_order").toInt(),
        numOrders = modInfoObject->value("num_orders").toInt();

    m_songStartLabel->setText(QString::number(currentOrder + 1));
    m_songEndLabel->setText(QString::number(numOrders + 1));

    if (m_songPositionSlider->tickInterval() != numOrders) {
        m_songPositionSlider->setMaximum(numOrders);
    }

    if (m_currentOrder != currentOrder || m_songPositionSlider->value() != currentOrder) {
        this->setSongPositionSliderValueSilent(m_currentOrder = currentOrder);
    }

}

void PlayerWidget::showPlayHidePauseButton() {
    m_playButton->show();
    m_pauseButton->hide();
}

void PlayerWidget::hidePauseShowPlayButton() {
    m_pauseButton->show();
    m_playButton->hide();
}

void PlayerWidget::setSongPositionSliderValueSilent(int value) {
    m_songPositionSlider->blockSignals(true);
    m_songPositionSlider->setValue(value);
    m_songPositionSlider->blockSignals(false);
}


// Destructor
PlayerWidget::~PlayerWidget()
{

}
