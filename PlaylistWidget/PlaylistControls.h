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

    QVector<QJsonObject *> m_filesInDir;

    QJsonDocument * m_currentPlaylistDocument;

    QDir m_dataDir;

    bool saveEmptyPlaylist(QString playlistName);

    void generateEmptyPlaylist(QString playlistName = "");
    void refreshComboFromDataDir();
    void connectPlaylistSelectorEvents();
    void disconnectPlaylistSelectorEvents();

public:
    explicit PlaylistControls(QWidget *parent = nullptr);

    QJsonArray *playlistSelectionObjects() const;
    void setPlaylistSelectionObjects(QJsonArray *playlistSelectionObjects);

signals:
    void onAddNewPlaylist(QJsonObject playlistObject);
    void onSelectPlaylist(QJsonObject playlistObject);
    void onDeletePlaylist(QJsonObject playlistObject);
    void onSavePlaylist(QJsonObject playlistObject);

    void onPlaylistSelectionRefreshPlaylist(QJsonArray files);

public slots:
    void onPlaylistSelectorChange(int itemIndex);
    void onNewPlaylistButtonPress();
};

#endif // PLAYLISTCONTROLS_H
