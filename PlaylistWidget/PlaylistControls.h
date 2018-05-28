#ifndef PLAYLISTCONTROLS_H
#define PLAYLISTCONTROLS_H

#include <QtWidgets>
#include <QtAwesome.h>

class PlaylistControls : public QWidget
{
    Q_OBJECT

private :
    QtAwesome *qtAwesome;
    QPushButton *buildButton(const char * iconType);
    QComboBox *m_playlistSelector;
    QJsonArray *m_playlistSelectionObjects;

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
};

#endif // PLAYLISTCONTROLS_H
