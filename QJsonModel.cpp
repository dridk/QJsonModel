/* QJsonModel.cpp
 * Copyright (c) 2011 SCHUTZ Sacha
 * Copyright © 2024 Saul D. Beniquez
 *
 * License:
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "QJsonModel.hpp"
#include "details/QJsonTreeItem.hpp"

#include <QDebug>
#include <QFile>
#include <QFont>

QJsonModel::QJsonModel(QObject* parent)
    : QAbstractItemModel(parent), mRootItem{ new QJsonTreeItem }
{
	mHeaders.append("key");
	mHeaders.append("value");
}

QJsonModel::QJsonModel(const QString& fileName, QObject* parent)
    : QAbstractItemModel(parent), mRootItem{ new QJsonTreeItem }
{
	mHeaders.append("key");
	mHeaders.append("value");
	load(fileName);
}

QJsonModel::QJsonModel(QIODevice* device, QObject* parent)
    : QAbstractItemModel(parent), mRootItem{ new QJsonTreeItem }
{
	mHeaders.append("key");
	mHeaders.append("value");
	load(device);
}

QJsonModel::QJsonModel(const QByteArray& json, QObject* parent)
    : QAbstractItemModel(parent), mRootItem{ new QJsonTreeItem }
{
	mHeaders.append("key");
	mHeaders.append("value");
	loadJson(json);
}

QJsonModel::~QJsonModel()
{
	delete mRootItem;
}

bool QJsonModel::load(const QString& fileName)
{
	QFile file(fileName);
	bool success = false;
	if (file.open(QIODevice::ReadOnly)) {
		success = load(&file);
		file.close();
	} else {
		success = false;
	}

	return success;
}

bool QJsonModel::load(QIODevice* device)
{
	return loadJson(device->readAll());
}

bool QJsonModel::loadJson(const QByteArray& json)
{
	auto const& jdoc = QJsonDocument::fromJson(json);

	if (!jdoc.isNull()) {
		beginResetModel();
		delete mRootItem;
		if (jdoc.isArray()) {
			mRootItem = QJsonTreeItem::load(
			    QJsonValue(jdoc.array()), mExceptions
			);
			mRootItem->setType(QJsonValue::Array);

		} else {
			mRootItem = QJsonTreeItem::load(
			    QJsonValue(jdoc.object()), mExceptions
			);
			mRootItem->setType(QJsonValue::Object);
		}
		endResetModel();
		return true;
	}

	qDebug() << Q_FUNC_INFO << "cannot load json";
	return false;
}

QVariant QJsonModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return {};

	QJsonTreeItem* item =
	    static_cast<QJsonTreeItem*>(index.internalPointer());

	if (role == Qt::DisplayRole) {
		if (index.column() == 0)
			return QString("%1").arg(item->key());

		if (index.column() == 1)
			return item->value();
	} else if (Qt::EditRole == role) {
		if (index.column() == 1)
			return item->value();
	}

	return {};
}

bool QJsonModel::setData(
    const QModelIndex& index, const QVariant& value, int role
)
{
	int col = index.column();
	if (Qt::EditRole == role) {
		if (col == 1) {
			QJsonTreeItem* item =
			    static_cast<QJsonTreeItem*>(index.internalPointer());
			item->setValue(value);
			emit dataChanged(index, index, { Qt::EditRole });
			return true;
		}
	}

	return false;
}

QVariant
QJsonModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole)
		return {};

	if (orientation == Qt::Horizontal)
		return mHeaders.value(section);
	else
		return {};
}

QModelIndex
QJsonModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return {};

	QJsonTreeItem* parentItem;

	if (!parent.isValid())
		parentItem = mRootItem;
	else
		parentItem =
		    static_cast<QJsonTreeItem*>(parent.internalPointer());

	QJsonTreeItem* childItem = parentItem->child(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return {};
}

QModelIndex QJsonModel::parent(const QModelIndex& index) const
{
	if (!index.isValid())
		return {};

	QJsonTreeItem* childItem =
	    static_cast<QJsonTreeItem*>(index.internalPointer());
	QJsonTreeItem* parentItem = childItem->parent();

	if (parentItem == mRootItem)
		return QModelIndex();

	return createIndex(parentItem->row(), 0, parentItem);
}

int QJsonModel::rowCount(const QModelIndex& parent) const
{
	QJsonTreeItem* parentItem;
	if (parent.column() > 0)
		return 0;

	if (!parent.isValid())
		parentItem = mRootItem;
	else
		parentItem =
		    static_cast<QJsonTreeItem*>(parent.internalPointer());

	return parentItem->childCount();
}

int QJsonModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 2;
}

Qt::ItemFlags QJsonModel::flags(const QModelIndex& index) const
{
	int col = index.column();
	auto item = static_cast<QJsonTreeItem*>(index.internalPointer());

	auto isArray = QJsonValue::Array == item->type();
	auto isObject = QJsonValue::Object == item->type();

	if ((col == 1) && !(isArray || isObject))
		return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
	else
		return QAbstractItemModel::flags(index);
}

QByteArray QJsonModel::json(bool compact)
{
	auto jsonValue = genJson(mRootItem);
	QByteArray json;
	if (jsonValue.isNull())
		return json;

	if (jsonValue.isArray())
		arrayToJson(jsonValue.toArray(), json, 0, compact);
	else
		objectToJson(jsonValue.toObject(), json, 0, compact);

	return json;
}

void QJsonModel::objectToJson(
    QJsonObject jsonObject, QByteArray& json, int indent, bool compact
)
{
	json += compact ? "{" : "{\n";
	objectContentToJson(
	    jsonObject, json, indent + (compact ? 0 : 1), compact
	);
	json += QByteArray(4 * indent, ' ');
	json += compact ? "}" : "}\n";
}
void QJsonModel::arrayToJson(
    QJsonArray jsonArray, QByteArray& json, int indent, bool compact
)
{
	json += compact ? "[" : "[\n";
	arrayContentToJson(jsonArray, json, indent + (compact ? 0 : 1), compact);
	json += QByteArray(4 * indent, ' ');
	json += compact ? "]" : "]\n";
}

void QJsonModel::arrayContentToJson(
    QJsonArray jsonArray, QByteArray& json, int indent, bool compact
)
{
	if (jsonArray.size() <= 0)
		return;

	QByteArray indentString(4 * indent, ' ');
	int i = 0;
	while (1) {
		json += indentString;
		valueToJson(jsonArray.at(i), json, indent, compact);
		if (++i == jsonArray.size()) {
			if (!compact)
				json += '\n';
			break;
		}
		json += compact ? "," : ",\n";
	}
}
void QJsonModel::objectContentToJson(
    QJsonObject jsonObject, QByteArray& json, int indent, bool compact
)
{
	if (jsonObject.size() <= 0)
		return;

	QByteArray indentString(4 * indent, ' ');
	int i = 0;
	while (1) {
		QString key = jsonObject.keys().at(i);
		json += indentString;
		json += '"';
		json += escapedString(key);
		json += compact ? "\":" : "\": ";
		valueToJson(jsonObject.value(key), json, indent, compact);
		if (++i == jsonObject.size()) {
			if (!compact)
				json += '\n';
			break;
		}
		json += compact ? "," : ",\n";
	}
}

void QJsonModel::valueToJson(
    QJsonValue jsonValue, QByteArray& json, int indent, bool compact
)
{
	QJsonValue::Type type = jsonValue.type();
	switch (type) {
		case QJsonValue::Bool:
			json += jsonValue.toBool() ? "true" : "false";
			break;
		case QJsonValue::Double: {
			const double value = jsonValue.toDouble();
			if (qIsFinite(value)) {
				json += QByteArray::number(
				    value, 'f', QLocale::FloatingPointShortest
				);
			} else {
				json += "null"; // +INF || -INF || NaN (see
				                // RFC4627#section2.4)
			}
			break;
		}
		case QJsonValue::String:
			json += '"';
			json += escapedString(jsonValue.toString());
			json += '"';
			break;
		case QJsonValue::Array:
			json += compact ? "[" : "[\n";
			arrayContentToJson(
			    jsonValue.toArray(),
			    json,
			    indent + (compact ? 0 : 1),
			    compact
			);
			json += QByteArray(4 * indent, ' ');
			json += ']';
			break;
		case QJsonValue::Object:
			json += compact ? "{" : "{\n";
			objectContentToJson(
			    jsonValue.toObject(),
			    json,
			    indent + (compact ? 0 : 1),
			    compact
			);
			json += QByteArray(4 * indent, ' ');
			json += '}';
			break;
		case QJsonValue::Null:
		default:
			json += "null";
	}
}

void QJsonModel::addException(const QStringList& exceptions)
{
	mExceptions = exceptions;
}

QJsonValue QJsonModel::genJson(QJsonTreeItem* item) const
{
	auto type = item->type();
	int nchild = item->childCount();

	if (QJsonValue::Object == type) {
		QJsonObject jsonObj;
		for (int i = 0; i < nchild; ++i) {
			auto child = item->child(i);
			auto key = child->key();
			jsonObj.insert(key, genJson(child));
		}
		return jsonObj;
	} else if (QJsonValue::Array == type) {
		QJsonArray arr;
		for (int i = 0; i < nchild; ++i) {
			auto child = item->child(i);
			arr.append(genJson(child));
		}
		return arr;
	} else {
		QJsonValue value;
		switch (item->value().typeId()) {
			case QMetaType::Bool: {
				value = item->value().toBool();
				break;
			}
			default:
				value = item->value().toString();
				break;
		}
		(item->value());
		return value;
	}
}
// clang-format off
// vim: set foldmethod=syntax foldminlines=10 textwidth=80 ts=4 sts=0 sw=8 noexpandtab ft=cpp.doxygen :
