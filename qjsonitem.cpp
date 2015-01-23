#include "qjsonitem.h"

QJsonTreeItem::QJsonTreeItem(QJsonTreeItem *parent)
{

    mParent = parent;


}

QJsonTreeItem::~QJsonTreeItem()
{
    qDeleteAll(mChilds);

}

void QJsonTreeItem::appendChild(QJsonTreeItem *item)
{
    mChilds.append(item);
}

QJsonTreeItem *QJsonTreeItem::child(int row)
{
    return mChilds.value(row);
}

QJsonTreeItem *QJsonTreeItem::parent()
{
    return mParent;
}

int QJsonTreeItem::childCount() const
{
    return mChilds.count();
}

int QJsonTreeItem::row() const
{
    if (mParent)
        return mParent->mChilds.indexOf(const_cast<QJsonTreeItem*>(this));

    return 0;
}

void QJsonTreeItem::setKey(const QString &key)
{
    mKey = key;
}

void QJsonTreeItem::setValue(const QString &value)
{
    mValue = value;
}

QString QJsonTreeItem::key() const
{
    return mKey;
}

QString QJsonTreeItem::value() const
{
    return mValue;
}

QJsonTreeItem* QJsonTreeItem::load(const QJsonValue& value, QJsonTreeItem* parent)
{


    QJsonTreeItem * rootItem = new QJsonTreeItem(parent);
    rootItem->setKey("root");

    if ( value.isObject())
    {

        //Get all QJsonValue childs
        foreach (QString key , value.toObject().keys()){

            QJsonValue v = value.toObject().value(key);
            QJsonTreeItem * child = load(v,rootItem);
            child->setKey(key);
            rootItem->appendChild(child);

        }

    }

    else if ( value.isArray())
    {
        //Get all QJsonValue childs
        int index = 0;
        foreach (QJsonValue v , value.toArray()){

            QJsonTreeItem * child = load(v,rootItem);

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

