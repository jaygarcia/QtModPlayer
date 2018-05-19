#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressDialog>

#include "Workers/ThreadedModFileCheck.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QProgressDialog progressDialog;

public:
    MainWindow(QWidget *parent = 0);
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);

    ~MainWindow();

public slots:
//    void onFileCountComplete(ThreadedModFileCheckResults);
};

#endif // MAINWINDOW_H
