#ifndef ASYNCBUFFEREDTABLEMODEL_H
#define ASYNCBUFFEREDTABLEMODEL_H


#include <QtConcurrent>

#include <QAbstractListModel>
static const int bufferSize(500);
static const int lookAhead(100);
static const int halfLookAhead(lookAhead/2);


class BufferedTableModel : public QAbstractTableModel {
    Q_OBJECT

private:
    void cacheRows(int, int) const;
    QJsonObject *fetchRow(int rowNumber, int playlistId) const;

    QVector<QJsonObject *> m_modFiles;
    QVector<QString> m_modFileNames;

    mutable QContiguousCache<QJsonObject *> m_rows;
    int m_count;
    int m_playlistId;

//    DBManager *m_dbManager;

public:
    BufferedTableModel(QObject *parent = 0);
    ~BufferedTableModel();

    void appendItems(QVector<QJsonObject *> m_modFiles);

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
