/* QJSonTreeItem.hpp
 *
 * Copyright (c) 2011 SCHUTZ Sacha
 * Copyright © 2024 Saul D. Beniquez
 *
 * License:
 * The MIT License (MIT)
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

#pragma once

#include <QAbstractItemModel>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

class QJsonTreeItem {
    public:
	QJsonTreeItem(QJsonTreeItem* parent = nullptr);
	~QJsonTreeItem();
	void appendChild(QJsonTreeItem* item);
	QJsonTreeItem* child(int row);
	QJsonTreeItem* parent();
	int childCount() const;
	int row() const;
	void setKey(const QString& key);
	void setValue(const QVariant& value);
	void setType(const QJsonValue::Type& type);
	QString key() const;
	QVariant value() const;
	QJsonValue::Type type() const;

	static QJsonTreeItem* load(
	    const QJsonValue& value, const QStringList& exceptions = {},
	    QJsonTreeItem* parent = nullptr
	);

    protected:
    private:
	QString mKey;
	QVariant mValue;
	QJsonValue::Type mType;
	QList<QJsonTreeItem*> mChilds;
	QJsonTreeItem* mParent = nullptr;
};
// clang-format off
// vim: set foldmethod=syntax foldminlines=10 textwidth=80 ts=8 sts=0 sw=8 noexpandtab ft=cpp.doxygen :
