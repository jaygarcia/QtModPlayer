#ifndef PLAYLISTWIDGET_H
#define PLAYLISTWIDGET_H

#include <QWidget>
#include <QtWidgets>

#include <QLayout>
#include <QProgressDialog>
#include <QTableWidget>
#include <QHeaderView>

#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>

#include "ThreadedModFileCheck.h"
#include <QtAwesome.h>



#include "bufferedtablemodel.h"
#include "PlaylistControls.h"



class PlaylistWidget : public QWidget
{
    Q_OBJECT

private:
    QProgressDialog m_progressDialog;
    QTableView *m_tableView;
    BufferedTableModel m_model;
    PlaylistControls *m_playlistControls;
//    QtAwesome *qtAwesome;

public:
    bool m_countingFiles;

    PlaylistWidget(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dropEvent(QDropEvent *e) override;
    void startFileInsertion(ThreadedModFileCheckResults *results);
    void refreshTableView();


signals:

public slots:
//    void onInserterPercentUpdate(int pctDone);
//    void onInserterComplete(int totalFiles);
};

#endif // PLAYLISTWIDGET_H
