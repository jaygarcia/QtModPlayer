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
    QtAwesome *m_qtAwesome;
    QPushButton *buildButton(const char * iconType, const char *label);

    QPushButton *m_newPlaylistButton,
                *m_savePlaylistButton,
                *m_deletePlaylistButton;

    QJsonArray *m_playlistSelectionObjects;
    QString m_defaultPlaylistName;

    QVector<QJsonObject *> m_playlistObjects;


    bool saveLoadedPlaylist();

    void generateEmptyPlaylist(QString playlistName = "");
    void connectPlaylistSelectorEvents();
    void disconnectPlaylistSelectorEvents();
    void addNewItemToSelectorAfterSave();

public:
    QComboBox *m_playlistSelector;

    explicit PlaylistControls(QWidget *parent = nullptr);
    void refreshComboWithData(QVector<QJsonObject *> playlistObjects);
    void seedComboData(QVector<QJsonObject *> playlistObjects);
    void selectPlaylistViaTableName(QString tableName);

    QJsonArray *playlistSelectionObjects() const;
    void setPlaylistSelectionObjects(QJsonArray *playlistSelectionObjects);
    QVector<QJsonObject *> copyJsonArrayToVector(QJsonArray inObject);
    void appendFilesToModel(QVector<QJsonObject *> files);
    void appendCurrentPlaylistToSelector();

    QPushButton *newPlaylistButton() const;
    QPushButton *savePlaylistButton() const;
    QPushButton *deletePlaylistButton() const;

    void setPlaylistObjects(const QVector<QJsonObject *> &playlistObjects);

signals:
    //    void onAddNewPlaylist(QJsonObject playlistObject);
    //    void onSelectPlaylist(QJsonObject playlistObject);
//    void onDeletePlaylist(QJsonObject playlistObject);
//    void onSavePlaylist(QJsonObject playlistObject);
    void playlistSelectionChange(QString playlistTableName);


public slots:
    void onPlaylistSelectorChangeIndex(int itemIndex);
//    void onNewPlaylistButtonPress();
//    void onSavePlaylistButtonPress();

};

#endif // PLAYLISTCONTROLS_H
