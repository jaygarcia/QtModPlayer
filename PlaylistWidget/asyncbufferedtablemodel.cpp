#include "asyncbufferedtablemodel.h"



RandomListModel::RandomListModel(QObject *parent)
    : QAbstractListModel(parent), m_rows(bufferSize), m_count(1000000) {
    m_dbManager = new DBManager(this);
}

RandomListModel::~RandomListModel() {
    this->m_dbManager->disconnect();
}

int RandomListModel::rowCount(const QModelIndex &) const {
    return m_count;
}

//! [0]
QVariant RandomListModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();


    int row = index.row();
    printf("%i\n",row);
    if (row > m_rows.lastIndex()) {
        if (row - m_rows.lastIndex() > lookAhead)
            cacheRows(row - halfLookAhead, qMin(m_count, row + halfLookAhead));
        else while (row > m_rows.lastIndex())
            m_rows.append(fetchRow(m_rows.lastIndex()+1));
    }
    else if (row < m_rows.firstIndex()) {
        if (m_rows.firstIndex() - row > lookAhead)
            cacheRows(qMax(0, row - halfLookAhead), row + halfLookAhead);
        else while (row < m_rows.firstIndex())
            m_rows.prepend(fetchRow(m_rows.firstIndex()-1));
    }

    return m_rows.at(row);
}

void RandomListModel::cacheRows(int from, int to) const {
    qDebug() << "From : " << from << " To " << to;
    for (int i = from; i <= to; ++i)
        m_rows.insert(i, fetchRow(i));
}
QString RandomListModel::fetchRow(int position) const {
    QString rowData =  QString::number(position) + " | row";
//    qDebug() << rowData;
    return rowData;
}



AsyncBufferedTableModel::AsyncBufferedTableModel(QObject *parent)
    : QAbstractTableModel(parent), m_rows(bufferSize), m_count(3000) {
}

AsyncBufferedTableModel::~AsyncBufferedTableModel() {
    // Disconnect from DB
}

int AsyncBufferedTableModel::rowCount(const QModelIndex &) const {
    return m_count;
}

//! [0]
QVariant AsyncBufferedTableModel::data(const QModelIndex &index, int role) const {
    if (role != Qt::DisplayRole)
        return QVariant();


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

    return m_rows.at(row);
}

void AsyncBufferedTableModel::cacheRows(int from, int to) const {
    qDebug() << "From : " << from << " To " << to;
    for (int i = from; i <= to; ++i)
        m_rows.insert(i, fetchRow(i));
}
QString AsyncBufferedTableModel::fetchRow(int position) const {
    QString rowData =  QString::number(position) + " | row";
    qDebug() << "Fetch row " << position;
    return rowData;
}
