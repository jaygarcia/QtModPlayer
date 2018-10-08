#ifndef WIDGETSTATESTORE_H
#define WIDGETSTATESTORE_H

#include <QObject>
#include <QJsonObject>
#include <QDebug>

class WidgetStateStore : public QObject
{
    Q_OBJECT


public:
    explicit WidgetStateStore(QObject *parent = nullptr);
    static QJsonObject *m_stateStore;

    static void RegisterStateObject(QJsonObject *stateObject);
    static QJsonObject GetStateObject(const QString &stateName);

signals:

public slots:

};


#endif // WIDGETSTATESTORE_H
