#include "PlaylistControls.h"

PlaylistControls::PlaylistControls(QWidget *parent) : QWidget(parent)
{
    qtAwesome = new QtAwesome(qApp);
    qtAwesome->initFontAwesome();

    QDir homeDir = QDir::home();

    QString dirName = ".QTModPlayer",
            dataDirName = QDir::home().absolutePath() + dirName;

    if (! homeDir.exists(dirName) && ! homeDir.mkdir(dirName)) {
        qWarning() << "Cannot make directory :: " << dirName;
    }

    m_dataDir = QDir(dataDirName);
}


void PlaylistControls::configure() {
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setAlignment(Qt::AlignCenter);
    this->setLayout(layout);

    QLabel *label = new QLabel(this);

    QFont font = QFont("Helvetica", 13, QFont::ExtraLight);
    label->setFont(font);
    label->setText("File:");
    layout->addWidget(label);

    QPushButton *addFile = this->buildButton("plus", "");
    layout->addWidget(addFile);

    QPushButton *removeFile = this->buildButton("minus", "");
    layout->addWidget(removeFile);

    layout->addItem(new QSpacerItem(40, 10));

    m_playlistSelector = new QComboBox(this);
    m_playlistSelector->setFont(QFont("Helvetica", 13, QFont::ExtraLight));
//    m_playlistSelector->setStyleSheet("QComboBox { padding: 5px }");

     //TODO: Continue here.
    // - Need method to write a file
    /*
     * {
     *     playlist_name : "",
     *     date_added : "",
     *     files : [
     *         {
     *             song_name : "",
     *             full_path : ""
     *         }
     *
     *     ]
     * }
     *
    QStringList *files = m_dataDir.entryInfoList(QDir::Files | QDir::Writable, QDir::Name);

    connect(m_playlistSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PlaylistControls::onPlaylistSelection);

    layout->addWidget(m_playlistSelector);

    layout->setStretch(4, 1);

    QPushButton *savePlaylistButton = this->buildButton("save", "");
    layout->addWidget(savePlaylistButton);


    QPushButton *deletePlaylistButton = this->buildButton("trash", "");
    layout->addWidget(deletePlaylistButton);
}

QPushButton *PlaylistControls::buildButton(const char *iconType, const char *label) {
    const char * baseButtonStyle = "color: #FEFEFE; height: 20px; padding: 1px; ";

    QVariantMap options;
    options.insert("color" , QColor(200,200,200));
    options.insert("color-active", QColor(200,200,200));

    QPushButton *button = new QPushButton(this->qtAwesome->icon(iconType, options), label);
    button->setStyleSheet(baseButtonStyle);

    return button;
}

QJsonArray *PlaylistControls::playlistSelectionObjects() const
{
    return m_playlistSelectionObjects;
}

void PlaylistControls::setPlaylistSelectionObjects(QJsonArray *playlistSelectionObjects)
{
    m_playlistSelectionObjects = playlistSelectionObjects;
    // Todo:: iterate and update combo box
}


void PlaylistControls::onPlaylistSelection(int itemIndex) {
    qDebug() << "Item selected " << itemIndex;
}
