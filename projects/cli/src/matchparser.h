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

#ifndef MATCHPARSER_H
#define MATCHPARSER_H

#include <QMap>
#include <QMultiMap>
#include <QStringList>
#include <QVariant>


/*!
 * \brief A command line parser for EngineMatch options
 *
 * \sa EngineMatch
 */
class MatchParser
{
	public:
		/*! A parsed command line option. */
		struct Option
		{
			/*!
			 * Returns a QMap containing a StringList option's
			 * parameters as name-value pairs.
			 *
			 * \a validArgs should contain all acceptable argument
			 * names for this option, separated by the '|' character
			 * (eg. "argument1|argument2").
			 *
			 * If this option's arguments don't match the ones in
			 * \a validArgs exactly, an empty map is returned.
			 */
			QMap<QString, QString> toMap(const QString& validArgs) const;

			QString name;	//!< The name of the option
			QVariant value;	//!< The value of the option
		};

		/*! Constructs a new parser for parsing \a args. */
		MatchParser(const QStringList& args);

		/*!
		 * Adds a new command line option.
		 *
		 * \param name The name of the option.
		 * \param type The storage type of the option's value.
		 *             For options with multiple arguments this
		 *             should be set to QVariant::QStringList.
		 * \param minArgs The minimum number of arguments.
		 * \param maxArgs The maximum number of arguments. A value
		 *                of -1 represents infinity.
		 * \param duplicates If true, there can be multiple
		 *                   instances of this option.
		 */
		void addOption(const QString& name,
			       QVariant::Type type,
			       int minArgs = 0,
			       int maxArgs = -1,
			       bool duplicates = false);
		/*!
		 * Returns the value of the option with the name \a name
		 * and removes the option from the parser.
		 *
		 * If \a name doesn't match any parsed option,
		 * a null QVariant is returned.
		 */
		QVariant takeOption(const QString& name);
		/*! Returns the options parsed by \a parse(). */
		QList<Option> options() const;
		/*!
		 * Parses the command line arguments.
		 * Returns true if successful; otherwise returns false.
		 */
		bool parse();

	private:
		struct PrivateOption
		{
			QVariant::Type type;
			int priority;
			int minArgs;
			int maxArgs;
			bool duplicates;
		};

		bool contains(const QString& optionName) const;

		QStringList m_args;
		QMultiMap<int, Option> m_options;
		QMap<QString, PrivateOption> m_validOptions;
		int m_priority;
};

#endif // MATCHPARSER_H
