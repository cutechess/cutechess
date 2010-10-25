/*
    This file is part of Cute Chess.

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "jsonserializer.h"
#include <QTextStream>


static QString jsonString(const QString& source)
{
	QString str;
	foreach (const QChar& c, source)
	{
		switch (c.toAscii())
		{
		case '\"':
			str += "\\\"";
			break;
		case '\\':
			str += "\\\\";
			break;
		case '\b':
			str += "\\b";
			break;
		case '\f':
			str += "\\f";
			break;
		case '\n':
			str += "\\n";
			break;
		case '\r':
			str += "\\r";
			break;
		case '\t':
			str += "\\t";
			break;
		default:
			if (c.unicode() >= 128)
			{
				QString u(QString::number(c.unicode(), 16));
				str += "\\u" + u.rightJustified(4, '0');
			}
			else
				str += c;
			break;
		}
	}

	return str;
}


JsonSerializer::JsonSerializer(const QVariant& data)
	: m_error(false),
	  m_data(data)
{
}

bool JsonSerializer::isError() const
{
	return m_error;
}

QString JsonSerializer::errorMessage() const
{
	return m_errorMessage;
}

void JsonSerializer::setError(const QString &message)
{
	if (m_error)
		return;
	m_error = true;
	m_errorMessage = message;
}

bool JsonSerializer::serializeNode(QTextStream& stream,
				   const QVariant& node,
				   int indentLevel)
{
	const QString indent(indentLevel, '\t');

	switch (node.type())
	{
	case QVariant::Invalid:
		stream << "null";
		break;
	case QVariant::Map:
		{
			stream << "{\n";

			const QVariantMap map(node.toMap());
			QVariantMap::const_iterator it;
			for (it = map.constBegin(); it != map.constEnd(); ++it)
			{
				stream << indent << "\t\"" << jsonString(it.key()) << "\" : ";
				if (!serializeNode(stream, it.value(), indentLevel + 1))
					return false;
				if (it != map.constEnd() - 1)
					stream << ',';
				stream << '\n';
			}

			stream << indent << '}';
		}
		break;
	case QVariant::List:
	case QVariant::StringList:
		{
			stream << "[\n";

			const QVariantList list(node.toList());
			for (int i = 0; i < list.size(); i++)
			{
				stream << indent << '\t';
				if (!serializeNode(stream, list.at(i), indentLevel + 1))
					return false;
				if (i != list.size() - 1)
					stream << ',';
				stream << '\n';
			}

			stream << indent << ']';
		}
		break;
	case QVariant::String:
	case QVariant::ByteArray:
		stream << '\"' << jsonString(node.toString()) << '\"';
		break;
	default:
		if (node.canConvert(QVariant::String))
			stream << node.toString();
		else
		{
			setError(QObject::tr("Invalid variant type: %1")
				 .arg(node.typeName()));
			return false;
		}
		break;
	}

	return true;
}

bool JsonSerializer::serialize(QTextStream& stream)
{
	bool ok = serializeNode(stream, m_data, 0);
	if (ok)
		stream << '\n';
	return ok;
}
