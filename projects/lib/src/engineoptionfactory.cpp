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

#include "engineoptionfactory.h"

#include "engineoption.h"
#include "enginetextoption.h"
#include "enginebuttonoption.h"
#include "enginecheckoption.h"
#include "enginecombooption.h"
#include "enginespinoption.h"

EngineOption* EngineOptionFactory::create(const QVariantMap& map)
{
	// TODO: use isValid() for each option

	const QString name = map["name"].toString();
	const QString type = map["type"].toString();
	const QVariant value = map["value"];
	QVariant defaultValue = map["default"];
	const QString alias = map["alias"].toString();

	if (name.isEmpty())
	{
		qWarning("Empty option name");
		return nullptr;
	}

	// Special case for the button option type: its value is the name
	if (type == "button")
		return new EngineButtonOption(name);

	if (value.type() != QVariant::Bool &&
		value.type() != QVariant::String &&
		value.type() != QVariant::Int)
	{
		qWarning("Invalid value type for option: %s",
			 qUtf8Printable(name));
		return nullptr;
	}

	if (defaultValue.isNull())
		defaultValue = value;
	else if (defaultValue.type() != QVariant::Bool &&
		defaultValue.type() != QVariant::String &&
		defaultValue.type() != QVariant::Int)
	{
		qWarning("Invalid default value type for option: %s",
			 qUtf8Printable(name));
		return nullptr;
	}

	// If the option type has not been defined, use text option as
	// a default option type
	if (type.isEmpty())
	{
		return new EngineTextOption(name, value.toString(),
			defaultValue.toString(), alias);
	}
	else if (type == "text" || type == "file" || type == "folder")
	{
		EngineTextOption::EditorType editorType;
		if (type == "file")
			editorType = EngineTextOption::FileDialog;
		else if (type == "folder")
			editorType = EngineTextOption::FolderDialog;
		else
			editorType = EngineTextOption::LineEdit;

		return new EngineTextOption(name, value.toString(),
			defaultValue.toString(), alias, editorType);
	}
	else if (type == "check")
	{
		return new EngineCheckOption(name, value.toBool(),
			defaultValue.toBool(), alias);
	}
	else if (type == "combo")
	{
		const QStringList choices(map["choices"].toStringList());
		if (choices.isEmpty())
			return nullptr;

		return new EngineComboOption(name, value.toString(),
			defaultValue.toString(), choices, alias);
	}
	else if (type == "spin")
	{
		int intValue, defaultIntValue, minValue, maxValue;
		bool ok;

		intValue = value.toInt(&ok);
		if (!ok)
			return nullptr;

		defaultIntValue = defaultValue.toInt(&ok);
		if (!ok)
			return nullptr;

		minValue = map["min"].toInt(&ok);
		if (!ok)
			return nullptr;

		maxValue = map["max"].toInt(&ok);
		if (!ok)
			return nullptr;

		return new EngineSpinOption(name, intValue, defaultIntValue,
			minValue, maxValue, alias);
	}

	return nullptr;
}
