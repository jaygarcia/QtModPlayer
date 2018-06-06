#include "BufferedTableModel.h"



BufferedTableModel::BufferedTableModel(QObject *parent)
    : QAbstractTableModel(parent), m_rows(bufferSize) {
    m_count = 0;

    m_dbManager = new DBManager(this);
    m_count = 0;
    m_dbManager->connect();
}


void BufferedTableModel::appendItems(QVector<QJsonObject *> modFiles) {
    this->beginResetModel();

    for (int i = 0; i < modFiles.size(); ++i) {
//        QJsonObject *modFile = new QJsonObject(QJsonObject::fromVariantHash(modFiles.at(i)->toVariantHash()));
        QJsonObject *modFile = modFiles.at(i);
        QString fullPath = QString(modFile->value("full_path").toString());

        if (! this->m_modFileNames.contains(fullPath)) {
            this->m_modFileNames.push_back(fullPath);
            this->m_modFiles.push_back(modFile);
        }
    }

    m_count = this->m_modFiles.size();

//    qDebug() << "m_count = " << m_count;

    this->endResetModel();
}

int BufferedTableModel::rowCount(const QModelIndex &) const {
    return m_count;
}

void BufferedTableModel::clearModel() {
    this->beginResetModel();
    m_rows.clear();
    m_modFiles.clear();
    m_modFileNames.clear();
//    m_count = m_dbManager->queryNumRowsForPlaylist(0);
    this->endResetModel();
}

QVariant BufferedTableModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    int row = index.row();

    if (row > m_rows.lastIndex()) {
        if (row - m_rows.lastIndex() > lookAhead) {
            cacheRows(row - halfLookAhead, qMin(m_count, row + halfLookAhead));
        }
        else while (row > m_rows.lastIndex()) {
            m_rows.append(fetchRow(m_rows.lastIndex()+1));
        }
    }
    else if (row < m_rows.firstIndex()) {
        if (m_rows.firstIndex() - row > lookAhead) {
            cacheRows(qMax(0, row - halfLookAhead), row + halfLookAhead);
        }
        else while (row < m_rows.firstIndex()) {
            m_rows.prepend(fetchRow(m_rows.firstIndex()-1));
        }
    }

    if (index.column() == 0) {
        return  m_rows.at(row)->value("song_name").toString();
    }
    else if (index.column() == 1) {
        return  m_rows.at(row)->value("file_name").toString();
    }

    return QVariant();
}

void BufferedTableModel::cacheRows(int from, int to) const {
    for (int i = from; i <= to; ++i)
        m_rows.insert(i, fetchRow(i));
}

QJsonObject * BufferedTableModel::fetchRow(int rowNumber) const {
//    ModFile *modFile = new ModFile();

//    return modFile;

    return this->m_modFiles.at(rowNumber);

//    QString rowData =  QString::number(position) + " | row";

//    QSqlRecord rowRecord = m_dbManager->getRecordAt(rowNumber + 1, playlistId);

//    return rowRecord;
}



BufferedTableModel::~BufferedTableModel() {

}

