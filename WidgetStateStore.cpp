#include "WidgetStateStore.h"

QJsonObject *WidgetStateStore::m_stateStore = new QJsonObject();


WidgetStateStore::WidgetStateStore(QObject *parent) : QObject(parent)
{

}

void WidgetStateStore::RegisterStateObject(QJsonObject *stateObject) {
    QString stateName = stateObject->value("name").toString();

    QJsonObject state;
    m_stateStore->insert(stateName, QJsonObject::fromVariantMap(stateObject->toVariantMap()));
}

QJsonObject WidgetStateStore::GetStateObject(const QString &stateName) {
    return m_stateStore->value(stateName).toObject();
}

//void WigetStateStore::UpdateState()
