#include "MainWindow.h"
#include <QApplication>

// include <QFile> and <QTextStream>
#include <QFile>
#include <QTextStream>
#include <QtAwesome.h>

#include <DarkStyle.h>
#include <QWidget>
#include <QObject>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setStyle(new DarkStyle);
    a.setQuitOnLastWindowClosed(false);

    // Enable high-DPI display support
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);



    MainWindow w;
    w.show();

    return a.exec();
}
