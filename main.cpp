#include "MainWindow.h"
#include <QApplication>

// include <QFile> and <QTextStream>
#include <QFile>
#include <QTextStream>
#include <QtAwesome.h>

#include <DarkStyle.h>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);
    a.setStyle(new DarkStyle);

    // Enable high-DPI display support
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    MainWindow w;
    w.show();

    return a.exec();
}
