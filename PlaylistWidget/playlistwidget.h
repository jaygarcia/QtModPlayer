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

#include "ThreadedModFileCheck.h"



#include "bufferedtablemodel.h"




class PlaylistWidget : public QWidget
{
    Q_OBJECT

private:
    QProgressDialog m_progressDialog;
    DBManager m_dbManager;
    QTableView *m_tableView;
    BufferedTableModel m_model;

public:
    bool m_countingFiles;

    ThreadedModFileInserter *m_modFileInserter;
    PlaylistWidget(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void startFileInsertion(ThreadedModFileCheckResults *results);
    void refreshTableView();
    void setModFileInserter(ThreadedModFileInserter *modFileInserter);

signals:

public slots:
//    void onInserterPercentUpdate(int pctDone);
//    void onInserterComplete(int totalFiles);
};

#endif // PLAYLISTWIDGET_H
