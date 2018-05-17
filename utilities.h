#ifndef UTILITIES_H
#define UTILITIES_H
#include <QObject>
#include <QtCore>


// Todo: include libopenmpt

class Utilities : public QObject
{
    Q_OBJECT
public:
    explicit Utilities();
    static void traverseDirectories(const QString dirName);



signals:

public slots:

};

#endif // UTILITIES_H
