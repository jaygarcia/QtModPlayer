#ifndef GLOBALSTATESTORE_H
#define GLOBALSTATESTORE_H

#include <QObject>

class GlobalStateStore
{
public:
    GlobalStateStore();

    static void registerStateObject(QString objectName, QJsonObject *stateObject);

};

#endif // GLOBALSTATESTORE_H
