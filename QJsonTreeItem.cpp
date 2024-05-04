/* QJsontreeItem.cpp
 *
 * Copyright (c) 2011 SCHUTZ Sacha
 * Copyright © 2024 Saul D. Beniquez
 *
 * License: The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "details/QJsonTreeItem.hpp"

QJsonTreeItem::QJsonTreeItem(QJsonTreeItem* parent)
{
	mParent = parent;
}

QJsonTreeItem::~QJsonTreeItem()
{
	qDeleteAll(mChilds);
}

void QJsonTreeItem::appendChild(QJsonTreeItem* item)
{
	mChilds.append(item);
}

QJsonTreeItem* QJsonTreeItem::child(int row)
{
	return mChilds.value(row);
}

QJsonTreeItem* QJsonTreeItem::parent()
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
		return mParent->mChilds.indexOf(const_cast<QJsonTreeItem*>(this)
		);

	return 0;
}

void QJsonTreeItem::setKey(const QString& key)
{
	mKey = key;
}

void QJsonTreeItem::setValue(const QVariant& value)
{
	mValue = value;
}

void QJsonTreeItem::setType(const QJsonValue::Type& type)
{
	mType = type;
}

QString QJsonTreeItem::key() const
{
	return mKey;
}

QVariant QJsonTreeItem::value() const
{
	return mValue;
}

QJsonValue::Type QJsonTreeItem::type() const
{
	return mType;
}

QJsonTreeItem* QJsonTreeItem::load(
    const QJsonValue& value, const QStringList& exceptions, QJsonTreeItem* parent
)
{
	QJsonTreeItem* rootItem = new QJsonTreeItem(parent);
	rootItem->setKey("root");

	auto contains = [](const QStringList& list, const QString& value) {
		for (auto val : list) {
			if (value.contains(val, Qt::CaseInsensitive)) {
				return true;
			}
		}
		return false;
	};

	if (value.isObject()) {
		// Get all QJsonValue childs
		const QStringList keys =
		    value.toObject().keys(); // To prevent clazy-range warning
		for (const QString& key : keys) {
			if (contains(exceptions, key)) {
				continue;
			}
			QJsonValue jsonValue = value.toObject().value(key);
			QJsonTreeItem* child =
			    load(jsonValue, exceptions, rootItem);
			child->setKey(key);
			child->setType(jsonValue.type());
			rootItem->appendChild(child);
		}
	} else if (value.isArray()) {
		// Get all QJsonValue childs
		int index = 0;
		const QJsonArray kArray =
		    value.toArray(); // To prevent clazy-range warning
		for (const QJsonValue& jsonValue : kArray) {
			QJsonTreeItem* child =
			    load(jsonValue, exceptions, rootItem);
			child->setKey(QString::number(index));
			child->setType(jsonValue.type());
			rootItem->appendChild(child);
			++index;
		}
	} else {
		rootItem->setValue(value.toVariant());
		rootItem->setType(value.type());
	}

	return rootItem;
}

