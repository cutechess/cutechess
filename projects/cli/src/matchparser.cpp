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

#include "matchparser.h"


MatchParser::MatchParser(const QStringList& args)
	: m_args(args)
{
}

void MatchParser::addOption(const QString& name,
			     QVariant::Type type,
			     int minArgs,
			     int maxArgs)
{
	PrivateOption option = { type, minArgs, maxArgs };
	m_validOptions[name] = option;
}

QMap<QString, QVariant> MatchParser::options() const
{
	return m_options;
}

bool MatchParser::parse()
{
	QStringList::const_iterator it;
	for (it = m_args.constBegin(); it != m_args.constEnd(); ++it)
	{
		if (!m_validOptions.contains(*it))
		{
			qWarning("Unknown option: \"%s\"", qPrintable(*it));
			return false;
		}
		QString name = *it;
		PrivateOption& option = m_validOptions[name];

		QStringList list;
		while (++it != m_args.constEnd())
		{
			if (it->size() > 1 && it->startsWith('-'))
			{
				bool ok = false;
				it->toDouble(&ok);
				if (!ok)
					break;
			}
			list << *it;
		}
		--it;
		
		if (m_options.contains(name))
		{
			qWarning("Multiple instances of option \"%s\"",
				 qPrintable(name));
			return false;
		}

		if (list.size() < option.minArgs)
		{
			if (option.maxArgs == option.minArgs)
				qWarning("Option \"%s\" needs %d argument(s)",
					 qPrintable(name), option.minArgs);
			else
				qWarning("Option \"%s\" needs at least %d argument(s)",
					 qPrintable(name), option.minArgs);
			return false;
		}
		if (option.maxArgs != -1 && list.size() > option.maxArgs)
		{
			qWarning("Too many arguments for option \"%s\"",
				 qPrintable(name));
			return false;
		}
		
		// Boolean option
		if (list.isEmpty())
		{
			m_options[name] = QVariant(true);
			continue;
		}
		
		QVariant value;
		if (option.type == QVariant::StringList)
			value.setValue(list);
		else
			value.setValue(list.join(" "));
		if (!value.convert(option.type))
		{
			qWarning("Invalid value for option \"%s\": \"%s\"",
				 qPrintable(name),
				 qPrintable(list.join(" ")));
			return false;
		}
		
		m_options[name] = value;
	}

	return true;
}
