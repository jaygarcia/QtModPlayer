#ifndef UISTATEOBJECT_H
#define UISTATEOBJECT_H

#include <QObject>
#include <QJsonObject>

#include "WidgetStateStore.h"

class UiStateObject : public QJsonObject
{
public:
    UiStateObject(const QString &name);
    void setState(const QString &key, const QJsonValue &value);
    QJsonValue getState(const QString &key);
};

#endif // UISTATEOBJECT_H
