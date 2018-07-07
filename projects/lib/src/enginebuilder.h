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

#ifndef ENGINEBUILDER_H
#define ENGINEBUILDER_H

#include "playerbuilder.h"
#include <QCoreApplication>
#include "engineconfiguration.h"


/*! \brief A class for constructing local chess engines. */
class LIB_EXPORT EngineBuilder : public PlayerBuilder
{
	Q_DECLARE_TR_FUNCTIONS(EngineBuilder)

	public:
		/*! Creates a new EngineBuilder. */
		EngineBuilder(const EngineConfiguration& config);

		// Inherited from PlayerBuilder
		virtual bool isHuman() const;
		virtual ChessPlayer* create(QObject* receiver,
					    const char* method,
					    QObject* parent,
					    QString* error) const;

	private:
		void setError(QString* error, const QString& message) const;

		EngineConfiguration m_config;
};

#endif // ENGINEBUILDER_H
