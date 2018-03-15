/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2011 SCHUTZ Sacha
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "qjsonmodel.h"

#include <QFile>
#include <QDebug>
#include <QFont>

QJsonTreeItem::QJsonTreeItem(QJsonTreeItem *parent)
{
    m_parent = parent;
}

QJsonTreeItem::~QJsonTreeItem()
{
    qDeleteAll(m_childs);
}

void QJsonTreeItem::appendChild(QJsonTreeItem *item)
{
    m_childs.append(item);
}

QJsonTreeItem *QJsonTreeItem::child(int row)
{
    return m_childs.value(row);
}

QJsonTreeItem *QJsonTreeItem::parent()
{
    return m_parent;
}

int QJsonTreeItem::childCount() const
{
    return m_childs.count();
}

int QJsonTreeItem::row() const
{
    if (m_parent)
        return m_parent->m_childs.indexOf(const_cast<QJsonTreeItem*>(this));

    return 0;
}

void QJsonTreeItem::setKey(const QString &key)
{
    m_key = key;
}

void QJsonTreeItem::setValue(const QString &value)
{
    m_value = value;
}

void QJsonTreeItem::setType(const QJsonValue::Type &type)
{
    m_type = type;
}

QString QJsonTreeItem::key() const
{
    return m_key;
}

QString QJsonTreeItem::value() const
{
    return m_value;
}

QJsonValue::Type QJsonTreeItem::type() const
{
    return m_type;
}

QJsonTreeItem* QJsonTreeItem::load(const QJsonValue &value, QJsonTreeItem *parent)
{
    QJsonTreeItem *rootItem = new QJsonTreeItem(parent);
    rootItem->setKey("root");

    if(value.isObject()) {
        // Get all QJsonValue childs
        for (QString key : value.toObject().keys()){
            QJsonValue v = value.toObject().value(key);
            QJsonTreeItem *child = load(v, rootItem);
            child->setKey(key);
            child->setType(v.type());
            rootItem->appendChild(child);
        }
        // Add bracket close item
        QJsonTreeItem *item = new QJsonTreeItem(rootItem);
        QString key = (parent) ? QString("},") : QString("}");
        item->setKey(key);
        rootItem->appendChild(item);
    } else if(value.isArray()) {
        // Get all QJsonValue childs
        int index = 0;
        for (QJsonValue v : value.toArray()){
            QJsonTreeItem *child = load(v, rootItem);
            child->setKey(QString::number(index));
            child->setType(v.type());
            rootItem->appendChild(child);
            ++index;
        }
        // Add array close item
        QJsonTreeItem *item = new QJsonTreeItem(rootItem);
        QString key = (parent) ? QString("],") : QString("]");
        item->setKey(key);
        rootItem->appendChild(item);
    } else {
        rootItem->setValue(value.toVariant().toString());
        rootItem->setType(value.type());
    }
    if(!parent) {
        // Add empty line item at the end
        QJsonTreeItem *item = new QJsonTreeItem(rootItem);
        item->setKey("");
        rootItem->appendChild(item);
    }

    return rootItem;
}

QJsonModel::QJsonModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_rootItem = new QJsonTreeItem;
    m_headers.append("Node");
    m_headers.append("Value");
}

QJsonModel::~QJsonModel()
{
    delete m_rootItem;
}

bool QJsonModel::load(const QString &fileName)
{
    QFile file(fileName);
    bool success = false;
    if (file.open(QIODevice::ReadOnly)) {
        success = load(&file);
        file.close();
    }
    else success = false;

    return success;
}

bool QJsonModel::load(QIODevice *device)
{
    return loadJson(device->readAll());
}

bool QJsonModel::loadJson(const QByteArray &json)
{
    m_document = QJsonDocument::fromJson(json);

    if (!m_document.isNull()) {
        beginResetModel();
        delete m_rootItem;
        if (m_document.isArray()) {
            m_rootItem = QJsonTreeItem::load(QJsonValue(m_document.array()));
        } else {
            m_rootItem = QJsonTreeItem::load(QJsonValue(m_document.object()));
        }
        endResetModel();
        return true;
    }

    qDebug()<<Q_FUNC_INFO<<"cannot load json";
    return false;
}

QVariant QJsonModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QJsonTreeItem *item = static_cast<QJsonTreeItem*>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return QString("%1").arg(item->key());
        if (index.column() == 1)
            return QString("%1").arg(item->value());
    }

    return QVariant();
}

QVariant QJsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {

        return m_headers.value(section);
    }
    else
        return QVariant();
}

QModelIndex QJsonModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    QJsonTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    QJsonTreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex QJsonModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    QJsonTreeItem *childItem = static_cast<QJsonTreeItem*>(index.internalPointer());
    QJsonTreeItem *parentItem = childItem->parent();

    if (parentItem == m_rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int QJsonModel::rowCount(const QModelIndex &parent) const
{
    QJsonTreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootItem;
    else
        parentItem = static_cast<QJsonTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int QJsonModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 2;
}
