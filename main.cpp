#include "mainwindow.h"
#include <QApplication>

// include <QFile> and <QTextStream>
#include <QFile>
#include <QTextStream>
#include <QtAwesome.h>

//#define DARKTHEME

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QtAwesome* awesome = new QtAwesome( qApp );
    awesome->initFontAwesome();     // This line is important as it loads the font and initializes the named icon map

#ifdef DARKTHEME
    // Begin: Load the stylesheet
    QFile f(":qdarkstyle/style.qss");
    if (!f.exists())     {
        printf("Unable to set stylesheet, file not found\n");
    }
    else
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());
    }
    // End: Load the stylesheet
#endif

    return a.exec();
}
