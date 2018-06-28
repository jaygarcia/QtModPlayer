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

    static QJsonObject CreateNewStateObject(QString widgetName);

    static void SetStateValue(QString stateName, QString stateValue);
    static void SetStateValue(QString stateName, char * stateValue);
    static void SetStateValue(QString stateName, int stateValue);
    static void SetStateValue(QString stateName, QJsonObject stateValue);
    static void SetStateValue(QString stateName, QJsonValue stateValue);

    static QJsonValue GetStateValue(QString stateName);
signals:

public slots:



};

#endif // WIDGETSTATESTORE_H
