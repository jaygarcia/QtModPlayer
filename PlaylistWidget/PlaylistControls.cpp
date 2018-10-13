#include "PlaylistControls.h"

#include "../UiStateObject.h"

PlaylistControls::PlaylistControls(QWidget *parent) : QWidget(parent)
{
    m_qtAwesome = new QtAwesome(qApp);
    m_qtAwesome->initFontAwesome();

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
    m_playlistSelector->setFont(font);
    connect(m_playlistSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PlaylistControls::onPlaylistSelectorChangeIndex);

    layout->addWidget(m_playlistSelector);

    layout->setStretch(4, 1);

    m_newPlaylistButton = this->buildButton("pluscircle", "");
    m_newPlaylistButton->setToolTip("Create playlist");
    layout->addWidget(m_newPlaylistButton);

    m_savePlaylistButton = this->buildButton("save", "");
    m_savePlaylistButton->setToolTip("Save playlist");
    layout->addWidget(m_savePlaylistButton);


    m_deletePlaylistButton = this->buildButton("trash", "");
    m_deletePlaylistButton->setToolTip("Delete playlist");
    layout->addWidget(m_deletePlaylistButton);


}

QPushButton *PlaylistControls::buildButton(const char *iconType, const char *label) {
    const char * baseButtonStyle = "color: #FEFEFE; height: 20px; padding: 1px; ";

    QVariantMap options;
    options.insert("color" , QColor(200,200,200));
    options.insert("color-active", QColor(200,200,200));

    QPushButton *button = new QPushButton(this->m_qtAwesome->icon(iconType, options), label);
    button->setStyleSheet(baseButtonStyle);

    return button;
}

void PlaylistControls::setPlaylistObjects(const QVector<QJsonObject *> &playlistObjects) {
    m_playlistObjects = playlistObjects;
}


bool PlaylistControls::saveLoadedPlaylist() {
//    QJsonObject rootObj = m_currentPlaylistDocument->object();
//    QString playlistName = rootObj.value("playlist_name").toString();

//    QJsonArray filesArray = QJsonArray(rootObj.value("files").toArray());

//    QFile file(m_dataDir.absolutePath() + "/" + playlistName + ".qmp");

//    file.open(QIODevice::WriteOnly);
//    file.write(m_currentPlaylistDocument->toBinaryData());
    return true;
}

void PlaylistControls::seedComboData(QVector<QJsonObject *> playlistObjects) {
    bool oldState = m_playlistSelector->blockSignals(true);
    m_playlistSelector->clear();

    this->m_playlistObjects = playlistObjects;

    m_playlistSelector->addItem("<Empty Playlist>","");

    int selectedTableId = globalStateObject->getState("selectedTableId").toInt();

    int selectedIndex = -1;


    for (int i = 0; i < playlistObjects.size(); ++i) {
        QJsonObject *playlistObj = playlistObjects.at(i);

        if (playlistObj->value("id") == selectedTableId) {
            selectedIndex = i;
        }


        m_playlistSelector->addItem(
            playlistObj->value("playlist_name").toString(),
            playlistObj->value("playlist_table_name").toString()
        );
    }

    m_playlistSelector->blockSignals(oldState);


    if (selectedIndex > -1) {
        m_playlistSelector->setCurrentIndex(selectedIndex);
    }


}
void PlaylistControls::refreshComboWithData(QVector<QJsonObject *> playlistObjects) {
    this->m_playlistObjects = playlistObjects;

    bool oldState = m_playlistSelector->blockSignals(true);
    m_playlistSelector->clear();
    m_playlistSelector->addItem("<Empty Playlist>","");

    int selectedIndex = -1;


//    qDebug() << "globalStateObject->getState(\"selectedTableName\").toString()" << globalStateObject->getState("selectedTableName").toString();
    int selectedTableId = globalStateObject->getState("selectedTableId").toInt();

    for (int i = 0; i < playlistObjects.size(); ++i) {
        QJsonObject *playlistObj = playlistObjects.at(i);

        QJsonValue selected = playlistObj->value("selected");

        if (selected != QJsonValue::Undefined) {
            selectedIndex = i + 1;
        }

        m_playlistSelector->addItem(
            playlistObj->value("playlist_name").toString(),
            playlistObj->value("playlist_table_name").toString()
        );
    }

    m_playlistSelector->blockSignals(oldState);

    if (selectedIndex > -1) {
        m_playlistSelector->setCurrentIndex(selectedIndex);
    }
}

void PlaylistControls::selectPlaylistViaTableName(QString tableName) {
    if (tableName.isEmpty() || tableName.isNull()) {
        return;
    }

    int numItems = m_playlistSelector->count();

    for (int i = 0; i < numItems; i++) {
        if (m_playlistSelector->itemData(i).toString().compare(tableName) > 0) {
            m_playlistSelector->setCurrentIndex(i - 1);
            return;
        }
    }
}

// Todo: Convert to DBManager usage
void PlaylistControls::appendCurrentPlaylistToSelector() {
//    QJsonObject rootObj = m_currentPlaylistDocument->object();
//    QString playlistName = rootObj.value("playlist_name").toString();

//    QJsonArray filesArray = QJsonArray(rootObj.value("files").toArray());

//    QFile file(m_dataDir.absolutePath() + "/" + playlistName + ".qmp");

//    int selectedIndex = this->m_playlistSelector->findData(m_dataDir.absolutePath() + "/" + playlistName + ".qmp");

//    this->disconnectPlaylistSelectorEvents();
//    this->m_playlistSelector->setCurrentIndex(selectedIndex);

//    QVector<QJsonObject *> filesVector = this->copyJsonArrayToVector(m_currentPlaylistDocument->object().value("files").toArray());
//    emit onPlaylistSelectionRefreshPlaylist(filesVector);

//    this->connectPlaylistSelectorEvents();
}


void PlaylistControls::onPlaylistSelectorChangeIndex(int itemIndex) {
//    qDebug() << "onPlaylistSelectorChangeIndex" << itemIndex << m_playlistSelector->itemData(itemIndex).toString();

    QString indexZeroItem = m_playlistSelector->itemText(0);
    QJsonObject *eventObject = new QJsonObject();
    QString tableName,
            playlistName;

    int tableId;

    if (indexZeroItem.isEmpty()) {
        bool oldState = m_playlistSelector->blockSignals(true);
        m_playlistSelector->removeItem(0);

        tableId = itemIndex - 1;
        m_playlistSelector->blockSignals(oldState);
        tableName = m_playlistSelector->itemData(tableId).toString();
        playlistName = m_playlistSelector->itemText(tableId);

    }
    else {
        tableName = m_playlistSelector->itemData(itemIndex).toString();
        playlistName = m_playlistSelector->itemText(itemIndex);
        tableId = itemIndex;
    }

    eventObject->insert("tableName", tableName);
    eventObject->insert("playlistName", playlistName);
    eventObject->insert("tableId", tableId);

    emit playlistSelectionChange(eventObject);

}




QJsonArray *PlaylistControls::playlistSelectionObjects() const{
    return m_playlistSelectionObjects;
}

void PlaylistControls::setPlaylistSelectionObjects(QJsonArray *playlistSelectionObjects) {
    m_playlistSelectionObjects = playlistSelectionObjects;
}



// Todo: Purge
void PlaylistControls::appendFilesToModel(QVector<QJsonObject *> incomingFiles) {
    Q_UNUSED(incomingFiles);
//    QJsonObject rootObj = m_currentPlaylistDocument->object();

//    QJsonArray filesArray = QJsonArray(rootObj.value("files").toArray());

//    for (int i = 0; i < incomingFiles.size(); ++i) {
//        QJsonObject *srcObj = incomingFiles.at(i);
//        QJsonObject destObj;

//        QStringList keys = srcObj->keys();
//        for (int k = 0; k < keys.size(); ++k) {
//            QString key = keys.at(k);
//            destObj.insert(key, srcObj->value(key));
//        }

//        filesArray.append(destObj);
//    }


//    rootObj.remove("files");
//    rootObj.insert("files", filesArray);

//    m_currentPlaylistDocument->setObject(rootObj);
}



QVector<QJsonObject *> PlaylistControls::copyJsonArrayToVector(QJsonArray jsonFiles) {
    QVector<QJsonObject *> returnVector;

    for (int i = 0; i < jsonFiles.size(); ++i) {
        QJsonObject *destObj = new QJsonObject(jsonFiles.at(i).toObject());
        returnVector.push_back(destObj);
    }

    return returnVector;
}




QPushButton *PlaylistControls::deletePlaylistButton() const
{
    return m_deletePlaylistButton;
}

QPushButton *PlaylistControls::savePlaylistButton() const
{
    return m_savePlaylistButton;
}

QPushButton *PlaylistControls::newPlaylistButton() const
{
    return m_newPlaylistButton;
}

