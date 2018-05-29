#ifndef PLAYLISTCONTROLS_H
#define PLAYLISTCONTROLS_H

#include <QtWidgets>
#include <QtAwesome.h>

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

    bool savePlaylist(QString playlistName);

    void generateEmptyPlaylist(QString playlistName = "");
    void refreshComboFromDataDir();

public:
    explicit PlaylistControls(QWidget *parent = nullptr);

    QJsonArray *playlistSelectionObjects() const;
    void setPlaylistSelectionObjects(QJsonArray *playlistSelectionObjects);

signals:
    void onAddNewPlaylist(QJsonObject playlistObject);
    void onSelectPlaylist(QJsonObject playlistObject);
    void onDeletePlaylist(QJsonObject playlistObject);
    void onSavePlaylist(QJsonObject playlistObject);

public slots:
    void onPlaylistSelection(int itemIndex);
    void onNewPlaylistButtonPress();
};

#endif // PLAYLISTCONTROLS_H
