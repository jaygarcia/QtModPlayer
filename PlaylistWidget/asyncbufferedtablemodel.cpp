#include "asyncbufferedtablemodel.h"


AsyncBufferedTableModel::AsyncBufferedTableModel(QObject *parent)
    : QAbstractTableModel(parent), m_rows(bufferSize) {
//    m_dbManager = new DBManager(this);
//    m_dbManager->queryNumRowsForPlaylist(0);
//    m_count = m_dbManager->queryNumRowsForPlaylist(0);
    m_count = 0;
//    m_dbManager->connect();
}


void AsyncBufferedTableModel::appendItems(QVector<ModFile *> modFiles) {
    this->beginResetModel();

    for (int i = 0; i < modFiles.size(); ++i) {
        ModFile *modFile = modFiles.at(i);

        if (! this->m_modFileNames.contains(modFile->full_path)) {
            this->m_modFileNames.push_back(modFile->full_path);
            this->m_modFiles.push_back(modFile);
        }
    }

    m_count = this->m_modFiles.size();

    this->endResetModel();
}

int AsyncBufferedTableModel::rowCount(const QModelIndex &) const {
    return m_count;
}

void AsyncBufferedTableModel::refresh() {
    this->beginResetModel();
    m_rows.clear();
//    m_count = m_dbManager->queryNumRowsForPlaylist(0);
    this->endResetModel();
}

QVariant AsyncBufferedTableModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    int row = index.row();

    if (row > m_rows.lastIndex()) {
        if (row - m_rows.lastIndex() > lookAhead) {
            cacheRows(row - halfLookAhead, qMin(m_count, row + halfLookAhead));
        }
        else while (row > m_rows.lastIndex()) {
            m_rows.append(fetchRow(m_rows.lastIndex()+1, 0));
        }
    }
    else if (row < m_rows.firstIndex()) {
        if (m_rows.firstIndex() - row > lookAhead) {
            cacheRows(qMax(0, row - halfLookAhead), row + halfLookAhead);
        }
        else while (row < m_rows.firstIndex()) {
            m_rows.prepend(fetchRow(m_rows.firstIndex()-1, 0));
        }
    }

    if (index.column() == 0) {
        return  m_rows.at(row)->song_name;
    }
    else if (index.column() == 1) {
        return  m_rows.at(row)->file_name;
    }

    return QVariant();
}

void AsyncBufferedTableModel::cacheRows(int from, int to) const {
    for (int i = from; i <= to; ++i)
        m_rows.insert(i, fetchRow(i, 0));
}

ModFile * AsyncBufferedTableModel::fetchRow(int rowNumber, int playlistId) const {
//    ModFile *modFile = new ModFile();

//    return modFile;

    return this->m_modFiles.at(rowNumber);

//    QString rowData =  QString::number(position) + " | row";

//    QSqlRecord rowRecord = m_dbManager->getRecordAt(rowNumber + 1, playlistId);

//    return rowRecord;
}



AsyncBufferedTableModel::~AsyncBufferedTableModel() {
    // Disconnect from DB
//    m_dbManager->disconnect();
//    m_dbManager->deleteLater();
}

