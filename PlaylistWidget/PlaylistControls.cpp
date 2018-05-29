#include "PlaylistControls.h"

#include <QErrorMessage>
// Begin copy from BOOST library: https://github.com/boostorg/boost/blob/master/LICENSE_1_0.txt
namespace
{
  const char invalid_chars[] =
    "\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
    "<>:\"/\\|";
  // note that the terminating '\0' is part of the string - thus the size below
  // is sizeof(invalid_chars) rather than sizeof(invalid_chars)-1.  I
  const std::string windows_invalid_chars(invalid_chars, sizeof(invalid_chars));

  const std::string valid_posix(
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._-");

} // unnamed namespace

namespace qfs
{


    #   ifdef BOOST_WINDOWS
    bool native(const std::string & name)
    {
      return windows_name(name);
    }
    #   else
    bool native(const std::string & name)
    {
      return  name.size() != 0
        && name[0] != ' '
        && name.find('/') == std::string::npos;
    }
    #   endif

    bool portable_posix_name(const std::string & name)
    {
      return name.size() != 0
        && name.find_first_not_of(valid_posix) == std::string::npos;
    }

    bool windows_name(const std::string & name)
    {
      return name.size() != 0
        && name[0] != ' '
        && name.find_first_of(windows_invalid_chars) == std::string::npos
        && *(name.end()-1) != ' '
        && (*(name.end()-1) != '.'
          || name.length() == 1 || name == "..");
    }

    bool portable_name(const std::string & name)
    {
      return
        name.size() != 0
        && (name == "."
          || name == ".."
          || (windows_name(name)
            && portable_posix_name(name)
            && name[0] != '.' && name[0] != '-'));
    }

    bool portable_directory_name(const std::string & name)
    {
      return
        name == "."
        || name == ".."
        || (portable_name(name)
          && name.find('.') == std::string::npos);
    }

    bool portable_file_name(const std::string & name)
    {
      std::string::size_type pos;
      return
         portable_name(name)
         && name != "."
         && name != ".."
         && ((pos = name.find('.')) == std::string::npos
             || (name.find('.', pos+1) == std::string::npos
               && (pos + 5) > name.length()))
        ;
    }

} // namespace filesystem


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

    this->refreshComboFromDataDir();


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
        //https://msdn.microsoft.com/en-us/library/aa365247
        std::string fn = text.toUtf8().constData();
        bool validName = qfs::portable_file_name(fn);

        if (!validName) {
            QMessageBox *msg = new QMessageBox(this);
            msg->setIcon(QMessageBox::Warning);
            msg->setText("Error: The file name entered has invalid characters. Please try again.");
            msg->show();

            connect(msg, &QMessageBox::buttonClicked, this, [this](QAbstractButton *button) {
                Q_UNUSED(button);
                this->onNewPlaylistButtonPress();
            });
        }
        else {
            this->generateEmptyPlaylist(text);;

            this->savePlaylist(text);
        }
       //        text.remove(QRegExp(QString::fromUtf8("`~!@#$%^&*()_—+=|:;<>«».?/\'\"\\\\/")));
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

void PlaylistControls::refreshComboFromDataDir() {
    // Todo: Populate combo box with files
    QFileInfoList files = m_dataDir.entryInfoList(QDir::Files | QDir::Writable, QDir::Name);
    qDebug()  << files.size();

    for (int i = 0; i < files.size(); ++i) {
        QFileInfo file = files.at(i);

        qDebug() << file.absoluteFilePath();
    }
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
