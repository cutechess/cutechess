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
	: m_args(args),
	  m_priority(0)
{
}

void MatchParser::addOption(const QString& name,
			    QVariant::Type type,
			    int minArgs,
			    int maxArgs,
			    bool duplicates)
{
	PrivateOption option =
	{
		type,
		m_priority++,
		minArgs,
		maxArgs,
		duplicates
	};
	m_validOptions[name] = option;
}

QVariant MatchParser::takeOption(const QString& name)
{
	Q_ASSERT(m_validOptions.contains(name));

	int i = m_validOptions.value(name).priority;
	if (m_options.contains(i))
		return m_options.take(i).value;

	return QVariant();
}

QList<MatchParser::Option> MatchParser::options() const
{
	QList<Option> list;

	const auto uniqueKeys = m_options.uniqueKeys();
	for (int key : uniqueKeys)
	{
		QList<Option> values(m_options.values(key));
		while (!values.isEmpty())
			list.append(values.takeLast());
	}

	return list;
}

bool MatchParser::contains(const QString& optionName) const
{
	QMultiMap<int, Option>::const_iterator it;
	for (it = m_options.constBegin(); it != m_options.constEnd(); ++it)
	{
		if (it.value().name == optionName)
			return true;
	}

	return false;
}

bool MatchParser::parse()
{
	QStringList::const_iterator it;
	for (it = m_args.constBegin(); it != m_args.constEnd(); ++it)
	{
		if (!m_validOptions.contains(*it))
		{
			qWarning("Unknown option: \"%s\"", qUtf8Printable(*it));
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
		
		if (!option.duplicates && contains(name))
		{
			qWarning("Multiple instances of option \"%s\"",
				 qUtf8Printable(name));
			return false;
		}

		if (list.size() < option.minArgs)
		{
			if (option.maxArgs == option.minArgs)
				qWarning("Option \"%s\" needs %d argument(s)",
					 qUtf8Printable(name), option.minArgs);
			else
				qWarning("Option \"%s\" needs at least %d argument(s)",
					 qUtf8Printable(name), option.minArgs);
			return false;
		}
		if (option.maxArgs != -1 && list.size() > option.maxArgs)
		{
			qWarning("Too many arguments for option \"%s\"",
				 qUtf8Printable(name));
			return false;
		}
		
		// Boolean option
		if (list.isEmpty())
		{
			Option tmp = { name, QVariant(true) };
			m_options.insert(option.priority, tmp);
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
				 qUtf8Printable(name),
				 qUtf8Printable(list.join(" ")));
			return false;
		}
		
		Option tmp = { name, value };
		m_options.insert(option.priority, tmp);
	}

	return true;
}

QMap<QString, QString> MatchParser::Option::toMap(const QString& validArgs) const
{
	const QStringList args = value.toStringList();
	QMap<QString, QString> defaults;
	QMap<QString, QString> map;

	const auto splitArgs = validArgs.split('|');
	for (const auto& arg : splitArgs)
	{
		QString argName = arg.section('=', 0, 0);
		QString argVal = arg.section('=', 1);

		if (argName.isEmpty() || argVal.isEmpty())
			defaults[arg] = QString();
		else
			defaults[argName] = argVal;
	}

	for (const auto& arg : args)
	{
		QString argName = arg.section('=', 0, 0);
		QString argVal = arg.section('=', 1);

		if (argName.isEmpty() || argVal.isEmpty()
		||  !defaults.contains(argName) || map.contains(argName))
		{
			qWarning("Invalid argument for option \"%s\": \"%s\"",
				 qUtf8Printable(name),
				 qUtf8Printable(arg));
			return QMap<QString, QString>();
		}

		map.insert(argName, argVal);
	}

	if (map.size() != defaults.size())
	{
		QStringList missing;
		QMap<QString, QString>::const_iterator it;
		for (it = defaults.constBegin(); it != defaults.constEnd(); ++it)
		{
			if (map.contains(it.key()))
				continue;

			if (it.value().isEmpty())
				missing.append(QString("\"%1\"").arg(it.key()));
			else
				map[it.key()] = it.value();
		}

		if (!missing.isEmpty())
		{
			qWarning("Option \"%s\" needs argument(s): %s",
				 qUtf8Printable(name),
				 qUtf8Printable(missing.join(", ")));
			return QMap<QString, QString>();
		}
	}

	return map;
}
