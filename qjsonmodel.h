#ifndef QJSONMODEL_H
#define QJSONMODEL_H

#include <QAbstractItemModel>
#include "jsonitem.h"
#include <QJsonDocument>
#include <QJsonObject>
class QJsonModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QJsonModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;


private:
    JsonItem * mRootItem;
    QJsonDocument mDocument;


};

#endif // QJSONMODEL_H
