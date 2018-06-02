#ifndef PLAYLISTCONTROLS_H
#define PLAYLISTCONTROLS_H

#include <QtWidgets>
#include <QtAwesome.h>
#include <QErrorMessage>

class PlaylistControls : public QWidget
{
    Q_OBJECT

private :
    void configure();
    QtAwesome *qtAwesome;
    QPushButton *buildButton(const char * iconType, const char *label);
    QComboBox *m_playlistSelector;
    QJsonArray *m_playlistSelectionObjects;
    QString m_defaultPlaylistName;

    QVector<QJsonObject *> m_filesInDir;

    QJsonDocument * m_currentPlaylistDocument;

    QDir m_dataDir;

    bool saveLoadedPlaylist();

    void generateEmptyPlaylist(QString playlistName = "");
    void refreshComboFromDataDir();
    void connectPlaylistSelectorEvents();
    void disconnectPlaylistSelectorEvents();
    void addNewItemToSelectorAfterSave();

public:
    explicit PlaylistControls(QWidget *parent = nullptr);

    QJsonArray *playlistSelectionObjects() const;
    void setPlaylistSelectionObjects(QJsonArray *playlistSelectionObjects);
    QVector<QJsonObject *> copyJsonArrayToVector(QJsonArray inObject);
    void appendFilesToModel(QVector<QJsonObject *> files);
    void appendCurrentPlaylistToSelector();

signals:
    void onAddNewPlaylist(QJsonObject playlistObject);
    void onSelectPlaylist(QJsonObject playlistObject);
    void onDeletePlaylist(QJsonObject playlistObject);
    void onSavePlaylist(QJsonObject playlistObject);

    void onPlaylistSelectionRefreshPlaylist(QVector<QJsonObject *> files);

public slots:
    void onPlaylistSelectorChange(int itemIndex);
    void onNewPlaylistButtonPress();
    void onSavePlaylistButtonPress();

};

#endif // PLAYLISTCONTROLS_H
