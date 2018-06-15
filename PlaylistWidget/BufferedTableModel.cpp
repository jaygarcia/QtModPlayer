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
    m_count = 0;
    this->endResetModel();
}

void BufferedTableModel::refresh(QString tableName) {
    this->beginResetModel();
    m_rows.clear();
    m_modFiles.clear();
    m_modFileNames.clear();
    if (! tableName.isNull()) {
        this->currentTableName = tableName;
        m_count = this->m_dbManager->getNumRowsForPlaylist(this->currentTableName);
    }
    else {
        m_count = 0;
    }

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
    QSqlRecord rowRecord = m_dbManager->getRecordAt(rowNumber + 1, this->currentTableName);

    QJsonObject *newObject = new QJsonObject();

    newObject->insert("file_name", rowRecord.value("file_name").toString());
    newObject->insert("song_name", rowRecord.value("song_name").toString());
    newObject->insert("full_path", rowRecord.value("full_path").toString());

//    qDebug() << Q_FUNC_INFO << rowRecord.value("file_name") << rowRecord.value("song_name");
    return newObject;
}




BufferedTableModel::~BufferedTableModel() {
    
}

