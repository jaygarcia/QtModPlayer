#ifndef PLAYLISTFILEMANAGER_H
#define PLAYLISTFILEMANAGER_H

#include <QObject>

class PlaylistFileManager : public QObject
{
    Q_OBJECT
public:
    explicit PlaylistFileManager(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PLAYLISTFILEMANAGER_H