#ifndef ASYNCBUFFEREDTABLEMODEL_H
#define ASYNCBUFFEREDTABLEMODEL_H


#include <QThread>
#include <tuple>
#include <QtConcurrent>
#include "../DBManager/dbmanager.h"

#include <QAbstractListModel>
static const int bufferSize(500);
static const int lookAhead(100);
static const int halfLookAhead(lookAhead/2);


//class AsyncBufferedTableModel : public QAbstractTableModel{
//    Q_OBJECT
//    //type used to hold the model's internal data in the variable m_rows
//    using RowsList = QList<std::tuple<QString, QString, QString> >;
//    //model's data
//    RowsList m_rows;
//    QFutureWatcher<RowsList>* m_watcher;

//public:
//    explicit AsyncBufferedTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent){
//        //start loading data in the thread pool as soon as the model is instantiated
//        m_watcher = new QFutureWatcher<RowsList>(this);

//        connect(m_watcher, &QFutureWatcher<RowsList>::finished,
//                this, &AsyncBufferedTableModel::updateData);

//        QFuture<RowsList> future = QtConcurrent::run(&AsyncBufferedTableModel::retrieveData);
//        m_watcher->setFuture(future);
//    }
//    ~AsyncBufferedTableModel() = default;

//    //this is a heavy function that returns data you want the model to display
//    //this is called in the thread pool using QtConcurrent::run
//    static RowsList retrieveData(){
//        //the function is heavy that it blocks the calling thread for 2 secs
//        qDebug() << "QThread::sleep(2);...";
////        QThread::sleep(2);
//        RowsList list;

//        for(int i=0; i < 10; i++){
//            printf("%i\n", i);;
//            list.append(std::make_tuple(QString("A%0").arg(i),
//                                        QString("B%0").arg(i),
//                                        QString("C%0").arg(i)));
//        }
//        qDebug() << "returning list";
//        return list;
//    }
//    //this is the slot that is called when data is finished loading
//    //it resets the model so that it displays new data
//    Q_SLOT void updateData(){
//        beginResetModel();
//        m_rows = m_watcher->future().result();
//        endResetModel();
//    }

//    int rowCount(const QModelIndex &parent) const {
//        if(parent.isValid()) return 0;
//        return m_rows.size();
//    }

//    int columnCount(const QModelIndex &parent) const {
//        if(parent.isValid()) return 0;
//        return 3;
//    }

//    QVariant data(const QModelIndex &index, int role) const {
//        QVariant value = QVariant();
//        switch(role){
//        case Qt::DisplayRole: case Qt::EditRole:
//            switch(index.column()){
//            case 0:
//                value = "File Name";
//                break;
//            case 1:
//                value = std::get<1>(m_rows[index.row()]);
//                break;
//            case 2:
//                value = std::get<2>(m_rows[index.row()]);
//            }
//            break;
//        }
//        return value;
//    }


//    QVariant headerData(int section, Qt::Orientation orientation, int role) const {
//        if (role != Qt::DisplayRole)
//            return QVariant();

//        if (orientation == Qt::Horizontal) {
//            switch (section) {
//                case 0:
//                    return "File Name";

//                case 1:
//                    return "Song Name";

//                default:
//                    return QVariant();
//            }
//        }
//        return QVariant();
//    }

//};




class AsyncBufferedTableModel : public QAbstractTableModel {
    Q_OBJECT

private:
    void cacheRows(int, int) const;
    QSqlRecord fetchRow(int rowNumber, int playlistId) const;

    mutable QContiguousCache<QSqlRecord> m_rows;
    int m_count;
    int m_playlistId;

    DBManager *m_dbManager;

public:
    AsyncBufferedTableModel(QObject *parent = 0);
    ~AsyncBufferedTableModel();

    int rowCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &, int) const override;

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
