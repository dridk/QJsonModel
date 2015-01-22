#include "jsonitem.h"

JsonItem::JsonItem(JsonItem *parent)
{

    mParent = parent;


}

JsonItem::~JsonItem()
{
    qDeleteAll(mChilds);

}

void JsonItem::appendChild(JsonItem *item)
{
    mChilds.append(item);
}

JsonItem *JsonItem::child(int row)
{
    return mChilds.value(row);
}

JsonItem *JsonItem::parent()
{
    return mParent;
}

int JsonItem::childCount() const
{
    return mChilds.count();
}

int JsonItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<JsonItem*>(this));

    return 0;
}

void JsonItem::setKey(const QString &key)
{
    mKey = key;
}

void JsonItem::setValue(const QString &value)
{
    mValue = value;
}

QString JsonItem::key() const
{
    return mKey;
}

QString JsonItem::value() const
{
    return mValue;
}

JsonItem* JsonItem::load(const QJsonValue& value, JsonItem* parent)
{


    JsonItem * rootItem = new JsonItem(parent);
    rootItem->setKey("root");

    if ( value.isObject())
    {

        //Get all QJsonValue childs
        foreach (QString key , value.toObject().keys()){

            QJsonValue v = value.toObject().value(key);
            JsonItem * child = load(v,rootItem);
            child->setKey(key);
            rootItem->appendChild(child);

        }

    }

    else if ( value.isArray())
    {
        //Get all QJsonValue childs
        int index = 0;
        foreach (QJsonValue v , value.toArray()){

            JsonItem * child = load(v,rootItem);

            child->setKey(QString::number(index));

            rootItem->appendChild(child);

            ++index;

        }

    }

    else
    {

        qDebug()<<value;
        rootItem->setValue(value.toVariant().toString());

    }

    return rootItem;



}

