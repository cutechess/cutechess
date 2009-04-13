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

#include "ucioption.h"
#include <QRegExp>
#include <QtDebug>
#include <QVector>


static UciOption::Type typeFromString(const QString& str)
{
	if (str == "check")
		return UciOption::Check;
	else if (str == "spin")
		return UciOption::Spin;
	else if (str == "combo")
		return UciOption::Combo;
	else if (str == "button")
		return UciOption::Button;
	else if (str == "string")
		return UciOption::String;
	return UciOption::NoType;
}

UciOption::UciOption()
	: m_type(NoType),
	  m_min(0),
	  m_max(0)
{
}

UciOption::UciOption(const QString& str)
{
	parse(str);
}

QVector<QStringList> UciOption::parse(const QString& str, QRegExp rx)
{
	QVector<QStringList> attrs;

	// Put the attributes' names and values in a vector
	QString item;
	int pos = 0;
	int prevPos= 0;
	while ((pos = rx.indexIn(str, pos)) != -1)
	{
		if (!item.isEmpty())
		{
			QString val = str.mid(prevPos + 1, pos - prevPos - 2);
			attrs.append((QStringList() << item << val.trimmed()));
		}
		item = rx.cap();
		pos += rx.matchedLength();
		prevPos = pos;
	}
	if (prevPos >= str.length() - 1)
		return attrs; // No value for the last attribute

	// Add the last attribute to the vector
	if (!item.isEmpty())
	{
		QString val = str.right(str.length() - prevPos - 1);
		attrs.append((QStringList() << item << val.trimmed()));
	}

	return attrs;
}

void UciOption::parse(const QString& str)
{
	m_type = NoType;
	m_min = 0;
	m_max = 0;

	QRegExp rx("\\b(name|type|default|min|max|var)\\b");
	QVector<QStringList> attrs = parse(str, rx);
	
	foreach (const QStringList& attr, attrs)
	{
		if (attr[0] == "name")
			setName(attr[1]);
		else if (attr[0] == "type")
			setType(typeFromString(attr[1]));
		else if (attr[0] == "default")
		{
			switch (m_type)
			{
			case Check:
				if (attr[1] == "true")
					setDefaultVal(true);
				else if (attr[1] == "false")
					setDefaultVal(false);
				break;
			case Spin:
				setDefaultVal(attr[1].toInt());
				break;
			case String:
			case Combo:
				setDefaultVal(attr[1]);
				break;
			default:
				break;
			};
		}
		else if (attr[0] == "min")
			setMin(attr[1].toInt());
		else if (attr[0] == "max")
			setMax(attr[1].toInt());
		else if (attr[0] == "var")
			addVar(attr[1]);
		else
		{
			m_type = NoType;
			return;
		}
	}
	
	if (!isOk())
		m_type = NoType;
}

QString UciOption::name() const
{
	return m_name;
}

UciOption::Type UciOption::type() const
{
	return m_type;
}

QVariant UciOption::defaultVal() const
{
	return m_defaultVal;
}

int UciOption::min() const
{
	return m_min;
}

int UciOption::max() const
{
	return m_max;
}

const QStringList& UciOption::var() const
{
	return m_var;
}

void UciOption::setName(const QString& name)
{
	m_name = name;
}

void UciOption::setType(Type type)
{
	m_type = type;
}

void UciOption::setDefaultVal(const QVariant& defaultVal)
{
	m_defaultVal = defaultVal;
}

void UciOption::setMin(int min)
{
	m_min = min;
}

void UciOption::setMax(int max)
{
	m_max = max;
}

void UciOption::addVar(const QString& str)
{
	m_var.append(str);
}

bool UciOption::isOk() const
{
	return isValueOk(m_defaultVal);
}

bool UciOption::isValueOk(const QVariant& value) const
{
	bool ok = false;
	int val = 0;
	
	if (m_name.isEmpty())
		return false;
	
	switch (m_type)
	{
	case Check:
		if (!value.canConvert<bool>())
			return false;
		if (m_min != 0 || m_max != 0)
			return false;
		if (!m_var.isEmpty())
			return false;
		return true;
	case Spin:
		if (m_min >= m_max)
			return false;
		val = value.toInt(&ok);
		if (!ok || val < m_min || val > m_max)
			return false;
		if (!m_var.isEmpty())
			return false;
		return true;
	case Combo:
		if (!m_var.contains(value.toString()))
			return false;
		if (m_min != 0 || m_max != 0)
			return false;
		return true;
	case Button:
		if (value.isValid() || !m_var.isEmpty())
			return false;
		if (m_min != 0 || m_max != 0)
			return false;
		return true;
	case String:
		if (m_min != 0 || m_max != 0)
			return false;
		if (!m_var.isEmpty())
			return false;
		return true;
	default:
		return false;
	};
}
