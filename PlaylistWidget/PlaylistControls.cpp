#include "PlaylistControls.h"

PlaylistControls::PlaylistControls(QWidget *parent) : QWidget(parent)
{
    qtAwesome = new QtAwesome(qApp);
    qtAwesome->initFontAwesome();

    QDir homeDir = QDir::home();

    QString dirName = ".QTModPlayer",
            dataDirName = QDir::home().absolutePath() + "/" + dirName;

    if (! homeDir.exists(dirName) && ! homeDir.mkdir(dirName)) {
        qWarning() << "Cannot make directory :: " << dirName;
    }



    m_dataDir = QDir(dataDirName);

    this->configure();
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

    QPushButton *addFileButton = this->buildButton("plus", "");
    addFileButton->setToolTip("Add file(s)");
    layout->addWidget(addFileButton);

    QPushButton *removeFileButton = this->buildButton("minus", "");
    removeFileButton->setToolTip("Remove File");
    layout->addWidget(removeFileButton);

    layout->addItem(new QSpacerItem(40, 10));

    m_playlistSelector = new QComboBox(this);
//    m_playlistSelector->setFont(QFont("Helvetica", 13, QFont::ExtraLight));
//    m_playlistSelector->setStyleSheet("QComboBox { padding: 5px }");

    // Todo: Populate combo box with files
    QFileInfoList files = m_dataDir.entryInfoList(QDir::Files | QDir::Writable, QDir::Name);
    qDebug()  << files.size();

    for (int i = 0; i < files.size(); ++i) {

    }


    connect(m_playlistSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PlaylistControls::onPlaylistSelection);

    layout->addWidget(m_playlistSelector);

    layout->setStretch(4, 1);

    QPushButton *newPlaylistButton = this->buildButton("pluscircle", "");
    newPlaylistButton->setToolTip("Create playlist");
    layout->addWidget(newPlaylistButton);
    connect(newPlaylistButton, &QPushButton::clicked, this, &PlaylistControls::onNewPlaylistButtonPress);

    QPushButton *savePlaylistButton = this->buildButton("save", "");
    savePlaylistButton->setToolTip("Save playlist");
    layout->addWidget(savePlaylistButton);


    QPushButton *deletePlaylistButton = this->buildButton("trash", "");
    deletePlaylistButton->setToolTip("Delete playlist");
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

void PlaylistControls::onNewPlaylistButtonPress() {
    bool okPressed;


    QString text = QInputDialog::getText(
        this,
        "New Playlist",
        "Enter a new playlist name:",
        QLineEdit::Normal,
        "",
        &okPressed
    );


    if (okPressed && !text.isEmpty()) {
        text.remove(QRegExp(QString::fromUtf8("[-`~!@#$%^&*()_—+=|:;<>«»,.?/{}\'\"\\\[\\\]\\\\]")));

        this->generateEmptyPlaylist(text);;

        this->savePlaylist(text);
//        qDebug() << text;
    }
}

void PlaylistControls::generateEmptyPlaylist(QString playlistName) {

    if (playlistName.isEmpty()) {
        playlistName = "Default playlist";
    }

    QJsonObject newPlaylist;
    newPlaylist.insert("playlist_name", playlistName);

    QJsonArray files;
    newPlaylist.insert("files", files);

    m_currentPlaylistDocument = new QJsonDocument(newPlaylist);

}

bool PlaylistControls::savePlaylist(QString playlistName) {

    QFile file(m_dataDir.absolutePath() + "/" + playlistName + ".qmp");

    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    out << m_currentPlaylistDocument->toJson();

    return true;
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
