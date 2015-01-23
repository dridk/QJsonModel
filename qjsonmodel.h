#ifndef QJSONMODEL_H
#define QJSONMODEL_H

#include <QAbstractItemModel>
#include "qjsonitem.h"
#include <QJsonDocument>
#include <QJsonObject>
class QJsonModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QJsonModel(QObject *parent = 0);
    bool load(const QString& fileName);
    bool load(QIODevice * device);
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column,const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &index) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;


private:
    QJsonTreeItem * mRootItem;
    QJsonDocument mDocument;
    QStringList mHeaders;


};

#endif // QJSONMODEL_H
