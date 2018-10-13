#include "UiStateObject.h"

UiStateObject *globalStateObject = new UiStateObject("global");


UiStateObject::UiStateObject(const QString &name) {
  this->insert("name", name);
//  qDebug() << Q_FUNC_INFO <<

  QJsonObject preExistingObject = WidgetStateStore::GetStateObject(name);
  if (! preExistingObject.isEmpty()) {
    // apply values from existing object
    QStringList keys = preExistingObject.keys();
    for (int i = 0; i < keys.count(); i++) {
//      qDebug() << "inserting " << keys.at(i) << preExistingObject.value(keys.at(i));

      this->insert(keys.at(i), preExistingObject.value(keys.at(i)));
    }
  }
  else {
    // Register new object
    WidgetStateStore::RegisterStateObject((QJsonObject*)this);
  }

}


void UiStateObject::setState(const QString &key, const QJsonValue &value) {
  this->insert(key, value);
  WidgetStateStore::RegisterStateObject((QJsonObject*)this);
}


QJsonValue UiStateObject::getState(const QString &key) {
//  qDebug() << Q_FUNC_INFO << key;
  return this->value(key);
}
