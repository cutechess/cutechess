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

#ifndef MATCHPARSER_H
#define MATCHPARSER_H

#include <QMap>
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
		/*! Returns the options parsed by \a parse(). */
		QList<Option> options() const;
		/*!
		 * Parses the command line arguments.
		 * Returns true if successfull.
		 */
		bool parse();

	private:
		struct PrivateOption
		{
			QVariant::Type type;
			int minArgs;
			int maxArgs;
			bool duplicates;
		};

		bool contains(const QString& optionName) const;

		QStringList m_args;
		QList<Option> m_options;
		QMap<QString, PrivateOption> m_validOptions;
};

#endif // MATCHPARSER_H
