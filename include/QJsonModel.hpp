/* QJsonModel.hpp
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

#pragma once

#include <QAbstractItemModel>
#include <QIcon>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "details/QUtf8.hpp"

class QJsonModel;
class QJsonItem;
class QJsonTreeItem;
class QFile;

class QJsonModel : public QAbstractItemModel {
	// NOLINTNEXTLINE
	Q_OBJECT
    public:
	// clang-format off
	enum FieldPermissions : unsigned char {
		ReadOnly      = 0b0000,
		WritableValue = 0b0001,
		WritableKey   = 0b0010
	}; // clang-format on

	explicit QJsonModel(
	    QObject* parent = nullptr,
	    FieldPermissions permissions = WritableValue
	);
	QJsonModel(
	    const QString& fileName, QObject* parent = nullptr,
	    FieldPermissions permissions = WritableValue
	);
	QJsonModel(
	    QIODevice* device, QObject* parent = nullptr,
	    FieldPermissions permissions = WritableValue
	);
	QJsonModel(
	    const QByteArray& json, QObject* parent = nullptr,
	    FieldPermissions permissions = WritableValue
	);

	~QJsonModel() override;

	using ErrorFlag = bool;
	ErrorFlag load(const QString& fileName);
	ErrorFlag load(QIODevice* file);
	ErrorFlag loadJson(const QByteArray& json);
	QVariant data(const QModelIndex& index, int role) const override;
	bool setData(
	    const QModelIndex& index, const QVariant& value,
	    int role = Qt::EditRole
	) override;
	QVariant headerData(int section, Qt::Orientation orientation, int role)
	    const override;
	QModelIndex index(
	    int row, int column, const QModelIndex& parent = QModelIndex()
	) const override;
	QModelIndex parent(const QModelIndex& index) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int
	columnCount(const QModelIndex& parent = QModelIndex()) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	QByteArray json(bool compact = false);
	QByteArray jsonToByte(QJsonValue jsonValue);
	void objectToJson(
	    QJsonObject jsonObject, QByteArray& json, int indent, bool compact
	);
	void arrayToJson(
	    QJsonArray jsonArray, QByteArray& json, int indent, bool compact
	);
	void arrayContentToJson(
	    QJsonArray jsonArray, QByteArray& json, int indent, bool compact
	);
	void objectContentToJson(
	    QJsonObject jsonObject, QByteArray& json, int indent, bool compact
	);
	void valueToJson(
	    QJsonValue jsonValue, QByteArray& json, int indent, bool compact
	);
	//! List of tags to skip during JSON parsing
	void addExclusion(const QStringList& exclusion);

    private:
	QJsonValue genJson(QJsonTreeItem*) const;
	QJsonTreeItem* rootItem = nullptr;
	QStringList headers;

	FieldPermissions editMode;
	//! List of line exclusions (e.g. comments).
	//! Case insensitive, compairs on "contains".
	QStringList exclusions;
};

inline uchar hexdig(uint positiveValue)
{
	return (
	    positiveValue < 0xa ? '0' + positiveValue : 'a' + positiveValue - 0xa
	);
}

inline QByteArray escapedString(const QString& text)
{
	QByteArray byteArray(text.length(), Qt::Uninitialized);
	uchar* cursor =
	    reinterpret_cast<uchar*>(const_cast<char*>(byteArray.constData()));
	const uchar* byteArrayEnd = cursor + byteArray.length();
	const ushort* src = reinterpret_cast<const ushort*>(text.constBegin());
	const ushort* const kend =
	    reinterpret_cast<const ushort*>(text.constEnd());
	while (src != kend) {
		if (cursor >= byteArrayEnd - 6) {
			// ensure we have enough space
			int pos = cursor - reinterpret_cast<const uchar*>(
					       byteArray.constData()
					   );
			byteArray.resize(byteArray.size() * 2);
			cursor =
			    reinterpret_cast<uchar*>(byteArray.data()) + pos;
			byteArrayEnd =
			    reinterpret_cast<const uchar*>(byteArray.constData()
			    ) +
			    byteArray.length();
		}
		uint uValue = *src++; // uValue = unsigned value
		if (uValue < 0x80) {
			if (uValue < 0x20 || uValue == 0x22 || uValue == 0x5c) {
				*cursor++ = '\\';
				switch (uValue) {
					case 0x22:
						*cursor++ = '"';
						break;
					case 0x5c:
						*cursor++ = '\\';
						break;
					case 0x8:
						*cursor++ = 'b';
						break;
					case 0xc:
						*cursor++ = 'f';
						break;
					case 0xa:
						*cursor++ = 'n';
						break;
					case 0xd:
						*cursor++ = 'r';
						break;
					case 0x9:
						*cursor++ = 't';
						break;
					default:
						*cursor++ = 'u';
						*cursor++ = '0';
						*cursor++ = '0';
						*cursor++ = hexdig(uValue >> 4);
						*cursor++ = hexdig(uValue & 0xf);
				}
			} else {
				*cursor++ = static_cast<uchar>(uValue);
			}
		} else if (QUtf8Functions::toUtf8<QUtf8BaseTraits>(uValue, cursor, src, kend) < 0) {
			// failed to get valid utf8 use JSON escape sequence
			*cursor++ = '\\';
			*cursor++ = 'u';
			*cursor++ = hexdig(uValue >> 12 & 0x0f);
			*cursor++ = hexdig(uValue >> 8 & 0x0f);
			*cursor++ = hexdig(uValue >> 4 & 0x0f);
			*cursor++ = hexdig(uValue & 0x0f);
		}
	}
	byteArray.resize(
	    cursor - reinterpret_cast<const uchar*>(byteArray.constData())
	);
	return byteArray;
}

// clang-format off
// vim: set foldmethod=syntax foldminlines=10 textwidth=80 ts=8 sts=0 sw=8 noexpandtab ft=cpp.doxygen :
