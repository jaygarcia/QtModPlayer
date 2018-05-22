#ifndef MODFILE_H
#define MODFILE_H

#include <QObject>

class ModFile : public QObject
{
    Q_OBJECT
public:
    explicit ModFile(QObject *parent = nullptr);
    QString song_name;
    QString file_name;
    QString file_name_short;
    QString parent_directory;
signals:

public slots:
};

#endif // MODFILE_H
