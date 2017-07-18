/*
    Copyright (c) 2010 Ilari Pihlajisto

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#include "jsonserializer.h"
#include <QTextStream>

namespace {

QString jsonString(const QString& source)
{
	QString str;
	for (const QChar& c : source)
	{
		switch (c.toLatin1())
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

} // anonymous namespace

JsonSerializer::JsonSerializer(const QVariant& data)
	: m_error(false),
	  m_data(data)
{
}

bool JsonSerializer::hasError() const
{
	return m_error;
}

QString JsonSerializer::errorString() const
{
	return m_errorString;
}

void JsonSerializer::setError(const QString &message)
{
	if (m_error)
		return;
	m_error = true;
	m_errorString = message;
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
			setError(tr("Invalid variant type: %1")
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
