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

#include "jsonparser.h"
#include <QTextStream>


QString JsonParser::tokenString(JsonParser::Token type, const QString& str)
{
	if (!str.isEmpty())
		return str;
	switch (type)
	{
	case JsonComma: return ",";
	case JsonColon: return ":";
	case JsonBeginObject: return "{";
	case JsonEndObject: return "}";
	case JsonBeginArray: return "[";
	case JsonEndArray: return "]";
	case JsonTrue: return "true";
	case JsonFalse: return "false";
	case JsonNull: return "null";
	case JsonString: return tr("(empty string)");
	default: return QString();
	}
}


JsonParser::JsonParser(QTextStream& stream)
	: m_error(false),
	  m_currentLine(1),
	  m_errorLine(0),
	  m_stream(stream)
{
}

bool JsonParser::hasError() const
{
	return m_error;
}

QString JsonParser::errorString() const
{
	return m_errorString;
}

qint64 JsonParser::errorLineNumber() const
{
	return m_errorLine;
}

void JsonParser::setError(const QString& message)
{
	if (m_error)
		return;

	m_error = true;
	m_errorString = message;
	m_errorLine = m_currentLine;
}

void JsonParser::clearError()
{
	if (!m_error)
		return;

	m_error = false;
	m_errorString.clear();
	m_errorLine = 0;
}

QVariant JsonParser::parse()
{
	return parseValue();
}

JsonParser::Token JsonParser::parseToken()
{
	static const QString termination(",]}");

	QChar c;
	bool escapeChar = false;
	bool inUnicode = false;
	QString unicode;
	Token type = JsonNone;

	m_lastToken.clear();

	while ((!m_stream.atEnd() || !m_buffer.isNull()) && !m_error)
	{
		if (m_buffer.isNull())
		{
			m_stream >> c;
			if (c == '\n')
				m_currentLine++;
		}
		else
		{
			c = m_buffer;
			m_buffer = QChar();
		}

		if (type == JsonNone && c.isSpace())
			continue;

		switch (type)
		{
		case JsonNone:
			switch (c.toLatin1())
			{
			case ',': return JsonComma;
			case ':': return JsonColon;
			case '{': return JsonBeginObject;
			case '}': return JsonEndObject;
			case '[': return JsonBeginArray;
			case ']': return JsonEndArray;
			case '\"':
				type = JsonString;
				break;
			default:
				type = JsonGeneric;
				m_lastToken += c;
				break;
			}
			break;
		case JsonString:
			if (escapeChar)
			{
				escapeChar = false;
				switch (c.toLatin1())
				{
				case '\"':
				case '\\':
				case '/':
					m_lastToken += c;
					break;
				case 'b':
					m_lastToken += '\b';
					break;
				case 'f':
					m_lastToken += '\f';
					break;
				case 'n':
					m_lastToken += '\n';
					break;
				case 'r':
					m_lastToken += '\r';
					break;
				case 't':
					m_lastToken += '\t';
					break;
				case 'u':
					inUnicode = true;
					unicode.clear();
					break;
				default:
					setError(tr("Unknown escape sequence: \\%1").arg(c));
					return JsonError;
				}
				break;
			}
			if (inUnicode)
			{
				if (!c.isLetterOrNumber())
				{
					setError(tr("Invalid unicode digit: %1").arg(c));
					return JsonError;
				}

				unicode += c;
				if (unicode.size() == 4)
				{
					bool ok = false;
					int code = unicode.toInt(&ok, 16);
					if (!ok)
					{
						setError(tr("Invalid unicode value: \\u%1")
							 .arg(unicode));
						return JsonError;
					}

					m_lastToken += QChar(code);
					unicode.clear();
					inUnicode = false;
				}
				break;
			}

			switch (c.toLatin1())
			{
			case '\\':
				escapeChar = true;
				break;
			case '\"':
				return type;
			default:
				m_lastToken += c;
				break;
			}
			break;
		case JsonGeneric:
			if (!c.isSpace() && !termination.contains(c))
			{
				m_lastToken += c;
				if (m_stream.atEnd())
					c = '\n';
				else
					break;
			}

			if (m_lastToken == "true")
				type = JsonTrue;
			else if (m_lastToken == "false")
				type = JsonFalse;
			else if (m_lastToken == "null")
				type = JsonNull;
			else if (m_lastToken.at(0).isDigit()
			     ||  m_lastToken.at(0) == '-')
				type = JsonNumber;
			else
			{
				setError(tr("Unknown token: %1")
					 .arg(m_lastToken));
				return JsonError;
			}

			m_buffer = c;
			return type;
		default:
			qFatal("UNREACHABLE");
		}
	}

	setError(tr("Reached EOF unexpectedly"));
	return JsonError;
}

QVariant JsonParser::parseValue(Token* tokenType)
{
	Token type = parseToken();
	if (tokenType != 0)
		*tokenType = type;
	if (type == JsonError || type == JsonNone || type == JsonGeneric)
		return QVariant();

	switch (type)
	{
	case JsonBeginObject:
		return parseObject();
	case JsonBeginArray:
		return parseArray();
	case JsonTrue:
		return QVariant(true);
	case JsonFalse:
		return QVariant(false);
	case JsonNull:
		return QVariant();
	case JsonNumber:
		{
			bool ok = false;
			if (m_lastToken.contains('.'))
			{
				double val = m_lastToken.toDouble(&ok);
				if (!ok)
				{
					setError(tr("Invalid fraction: %1")
						 .arg(m_lastToken));
					return QVariant();
				}
				return val;
			}
			else
			{
				int val = m_lastToken.toInt(&ok);
				if (ok)
					return val;
				qlonglong longval = m_lastToken.toLongLong(&ok);
				if (ok)
					return longval;

				setError(tr("Invalid integer: %1")
					 .arg(m_lastToken));
				return QVariant();
			}
		}
	case JsonString:
		return QVariant(m_lastToken);
	default:
		setError(tr("Invalid value: %1")
			 .arg(tokenString(type, m_lastToken)));
		return QVariant();
	}
}

QVariant JsonParser::parseObject()
{
	Token t = JsonNone;
	QString name;
	QVariant value;
	QVariantMap map;

	for (;;)
	{
		t = parseToken();
		if (t == JsonEndObject)
		{
			if (!map.isEmpty())
			{
				setError(tr("Expected more key/value pairs"));
				break;
			}
			return map;
		}
		if (t != JsonString)
		{
			setError(tr("Invalid key: %1")
				 .arg(tokenString(t, m_lastToken)));
			break;
		}
		name = m_lastToken;

		t = parseToken();
		if (t != JsonColon)
		{
			setError(tr("Expected colon instead of: %1")
				 .arg(tokenString(t, m_lastToken)));
			break;
		}

		value = parseValue();
		if (m_error)
			break;

		map[name] = value;
		
		t = parseToken();
		if (t == JsonEndObject)
			return map;
		if (t != JsonComma)
		{
			setError(tr("Expected comma or closing bracket instead of: %1")
				 .arg(tokenString(t, m_lastToken)));
			break;
		}
	}

	return QVariant();
}

QVariant JsonParser::parseArray()
{
	Token t = JsonNone;
	QVariant value;
	QVariantList list;

	for (;;)
	{
		value = parseValue(&t);

		if (t == JsonError)
			break;
		if (t == JsonEndArray)
		{
			clearError();
			if (!list.isEmpty())
			{
				setError(tr("Expected more array items"));
				break;
			}
			return list;
		}
		list << value;

		t = parseToken();
		if (t == JsonEndArray)
			return list;
		if (t != JsonComma)
		{
			setError(tr("Expected comma or closing bracket instead of: %1")
				 .arg(tokenString(t, m_lastToken)));
			break;
		}
	}

	return QVariant();
}
