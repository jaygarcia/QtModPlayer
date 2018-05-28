#ifndef ASYNCBUFFEREDTABLEMODEL_H
#define ASYNCBUFFEREDTABLEMODEL_H


//#include <QThread>
//#include <tuple>
#include <QtConcurrent>
//#include "../DBManager/dbmanager.h"
#include "../modfile.h"

#include <QAbstractListModel>
static const int bufferSize(500);
static const int lookAhead(100);
static const int halfLookAhead(lookAhead/2);


class AsyncBufferedTableModel : public QAbstractTableModel {
    Q_OBJECT

private:
    void cacheRows(int, int) const;
    ModFile *fetchRow(int rowNumber, int playlistId) const;

    QVector<ModFile *> m_modFiles;
    QVector<QString> m_modFileNames;

    mutable QContiguousCache<ModFile *> m_rows;
    int m_count;
    int m_playlistId;

//    DBManager *m_dbManager;

public:
    AsyncBufferedTableModel(QObject *parent = 0);
    ~AsyncBufferedTableModel();

    void appendItems(QVector<ModFile *> m_modFiles);

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &, int) const override;

    void refresh();

    int columnCount(const QModelIndex &parent) const override {
        Q_UNUSED(parent);
        return 2;
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {
        if (role != Qt::DisplayRole)
            return QVariant();

        if (orientation == Qt::Horizontal) {
            switch (section) {
                case 0:
                    return "Song Name";

                case 1:
                    return "File Name";

                default:
                    return QVariant();
            }
        }
        return QVariant();
    }

};



#endif // ASYNCBUFFEREDTABLEMODEL_H
