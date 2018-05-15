#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtWidgets>
#include <QtAwesome.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QLabel *songLabel;
    QtAwesome *qtAwesome;

    void configure();
    void addChildren();

    QWidget *buildSongInformationUI();
    QWidget *buildPlayerControlUI();
    QPushButton *buildButton(const char *iconType);
    QWidget *buildBottomControlUI();

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
};

#endif // MAINWINDOW_H
