#include "WidgetStateStore.h"

QJsonObject *WidgetStateStore::m_stateStore = new QJsonObject();


WidgetStateStore::WidgetStateStore(QObject *parent) : QObject(parent)
{

}

QJsonObject WidgetStateStore::CreateNewStateObject(QString widgetName) {
    if (! m_stateStore->contains(widgetName)) {
        QJsonObject state;
        qDebug() << "new state for " << widgetName << state;
        m_stateStore->insert(widgetName, state);

        return state;
    }
    else {
        qDebug() <<  << "Found state object for " << widgetName;

        return m_stateStore->value(widgetName).toObject();
    }
}
