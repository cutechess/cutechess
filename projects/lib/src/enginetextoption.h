/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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

#ifndef ENGINETEXTOPTION_H
#define ENGINETEXTOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineTextOption : public EngineOption
{
	public:
		enum EditorType
		{
			LineEdit,
			FileDialog,
			FolderDialog
		};

		EngineTextOption(const QString& name,
		                 const QVariant& value = QVariant(),
		                 const QVariant& defaultValue = QVariant(),
				 const QString& alias = QString(),
				 EditorType editorType = LineEdit);

		// Inherited from EngineOption
		virtual EngineOption* copy() const;
		virtual bool isValid(const QVariant& value) const;
		virtual QVariant toVariant() const;

	private:
		EditorType m_editorType;
};

#endif // ENGINETEXTOPTION_H
