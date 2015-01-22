#ifndef JSONITEM_H
#define JSONITEM_H
#include <QtCore>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
class JsonItem
{
public:
    JsonItem(JsonItem * parent = 0);
    ~JsonItem();
    void appendChild(JsonItem * item);
    JsonItem *child(int row);
    JsonItem *parent();
    int childCount() const;
    int row() const;
    void setKey(const QString& key);
    void setValue(const QString& value);

    QString key() const;
    QString value() const;

    static JsonItem* load(const QJsonValue& value, JsonItem * parent = 0);

protected:


private:
    QString mKey;
    QString mValue;
    QJsonValue::Type mType;

    QList<JsonItem*> mChilds;
    JsonItem * mParent;


};

#endif // JSONITEM_H
