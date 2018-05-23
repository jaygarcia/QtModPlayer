#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QLayout>
#include <QProgressDialog>
#include <QTableView>

#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QTableView>

#include "../Workers/ThreadedModFileCheck.h"
#include "../DBManager/dbmanager.h"
#include "../DBManager/ThreadedModFileInserter.h"




#include <QThread>
#include <tuple>
#include <QtConcurrent>

class AsyncTableModel : public QAbstractTableModel{
    Q_OBJECT
    //type used to hold the model's internal data in the variable m_rows
    using RowsList = QList<std::tuple<QString, QString, QString> >;
    //model's data
    RowsList m_rows;
    QFutureWatcher<RowsList>* m_watcher;

public:
    explicit AsyncTableModel(QObject* parent = nullptr) : QAbstractTableModel(parent){
        //start loading data in the thread pool as soon as the model is instantiated
        m_watcher = new QFutureWatcher<RowsList>(this);

        connect(m_watcher, &QFutureWatcher<RowsList>::finished,
                this, &AsyncTableModel::updateData);

        QFuture<RowsList> future = QtConcurrent::run(&AsyncTableModel::retrieveData);
        m_watcher->setFuture(future);

    }
    ~AsyncTableModel() = default;

    //this is a heavy function that returns data you want the model to display
    //this is called in the thread pool using QtConcurrent::run
    static RowsList retrieveData(){
        //the function is heavy that it blocks the calling thread for 2 secs
//        qDebug() << "QThread::sleep(2);...";
//        QThread::sleep(0);
        RowsList list;
        for(int i=0; i < 10; i++){
            printf("%i\n", i);;
            list.append(std::make_tuple(QString("A%0").arg(i),
                                        QString("B%0").arg(i),
                                        QString("C%0").arg(i)));
        }
        qDebug() << "returning list";
        return list;
    }
    //this is the slot that is called when data is finished loading
    //it resets the model so that it displays new data
    Q_SLOT void updateData(){
        beginResetModel();
        m_rows = m_watcher->future().result();
        endResetModel();
    }

    int rowCount(const QModelIndex &parent) const {
        if(parent.isValid()) return 0;
        return m_rows.size();
    }

    int columnCount(const QModelIndex &parent) const {
        if(parent.isValid()) return 0;
        return 3;
    }

    QVariant data(const QModelIndex &index, int role) const {
        QVariant value= QVariant();
        switch(role){
        case Qt::DisplayRole: case Qt::EditRole:
            switch(index.column()){
            case 0:
                value= std::get<0>(m_rows[index.row()]);
                break;
            case 1:
                value= std::get<1>(m_rows[index.row()]);
                break;
            case 2:
                value= std::get<2>(m_rows[index.row()]);
            }
            break;
        }
        return value;
    }
};


class PlaylistWidget : public QWidget
{
    Q_OBJECT

private:
    QProgressDialog m_progressDialog;
    DBManager m_dbManager;
    ThreadedModFileInserter m_modFileInserter;
    QTableView *m_tableView;

public:
    PlaylistWidget(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

signals:

public slots:
};

#endif // PLAYLISTWIDGET_H
