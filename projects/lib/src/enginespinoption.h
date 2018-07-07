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

#ifndef ENGINESPINOPTION_H
#define ENGINESPINOPTION_H

#include "engineoption.h"

class LIB_EXPORT EngineSpinOption : public EngineOption
{
	public:
		EngineSpinOption(const QString& name,
		                 const QVariant& value = QVariant(),
		                 const QVariant& defaultValue = QVariant(),
		                 int min = 0,
		                 int max = 0,
		                 const QString& alias = QString());

		// Inherited from EngineOption
		virtual EngineOption* copy() const;
		virtual bool isValid(const QVariant& value) const;
		virtual QVariant toVariant() const;

		int min() const;
		int max() const;

		void setMin(int min);
		void setMax(int max);

	private:
		int m_min;
		int m_max;
};

#endif // ENGINESPINOPTION_H
