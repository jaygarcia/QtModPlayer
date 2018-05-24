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


#include "asyncbufferedtablemodel.h"




class PlaylistWidget : public QWidget
{
    Q_OBJECT

private:
    QProgressDialog m_progressDialog;
    DBManager m_dbManager;
    ThreadedModFileInserter m_modFileInserter;
    QTableView *m_tableView;
    AsyncBufferedTableModel m_model;
public:
    PlaylistWidget(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;

signals:

public slots:
};

#endif // PLAYLISTWIDGET_H
