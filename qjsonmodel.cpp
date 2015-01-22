#include "qjsonmodel.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

QJsonModel::QJsonModel(QObject *parent) :
    QAbstractItemModel(parent)
{

    QFile file("/Users/sacha/example.json");

    file.open(QIODevice::ReadOnly);
    mDocument = QJsonDocument::fromJson(file.readAll());

    mRootItem = JsonItem::load(QJsonValue(mDocument.object()));

}

QVariant QJsonModel::data(const QModelIndex &index, int role) const
{

    if (!index.isValid())
           return QVariant();

       if (role != Qt::DisplayRole)
           return QVariant();

       JsonItem *item = static_cast<JsonItem*>(index.internalPointer());

       return QString("%1:%2").arg(item->key()).arg(item->value());

}

QModelIndex QJsonModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    JsonItem *parentItem;

    if (!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<JsonItem*>(parent.internalPointer());

    JsonItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QJsonModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
          return QModelIndex();

      JsonItem *childItem = static_cast<JsonItem*>(index.internalPointer());
      JsonItem *parentItem = childItem->parent();

      if (parentItem == mRootItem)
          return QModelIndex();

      return createIndex(parentItem->row(), 0, parentItem);
}

int QJsonModel::rowCount(const QModelIndex &parent) const
{
    JsonItem *parentItem;
       if (parent.column() > 0)
           return 0;

       if (!parent.isValid())
           parentItem = mRootItem;
       else
           parentItem = static_cast<JsonItem*>(parent.internalPointer());

       return parentItem->childCount();
}

int QJsonModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}
