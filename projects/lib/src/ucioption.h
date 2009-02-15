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

#ifndef UCI_OPTION_H
#define UCI_OPTION_H

#include <QVariant>
#include <QStringList>


/*!
 * \brief Configuration option for UCI chess engines
 *
 * UCI engines send a list of options and their default values
 * at startup. This class is used to prepare an option which can
 * be sent to an engine, to parse an option string sent by an
 * engine, and to check an option for errors.
 */
class LIB_EXPORT UciOption
{
	public:
		/*! Option (widget) type. */
		enum Type
		{
			Check,	//!< Boolean (Check Box)
			Spin,	//!< Integer (Spin Box)
			Combo,	//!< String (Combo Box)
			Button,	//!< Action (Push Button)
			String,	//!< String (Text Edit)
			NoType	//!< NoType/Invalid type
		};
		
		/*! Creates a new, empty option of NoType. */
		UciOption();
		/*! Creates a new option from a UCI option string. */
		explicit UciOption(const QString& str);
		
		/*! Parses a UCI option string, and resets the option. */
		void parse(const QString& str);

		/*! Returns the option name. May contain spaces. */
		QString name() const;
		/*! Returns the option type. */
		Type type() const;
		/*!
		 * Returns the default option value.
		 *
		 * Depending on the option type, the value can be:
		 * - bool (Check)
		 * - int (Spin)
		 * - QString (String, Combo)
		 * - Null QVariant (Button, NoType)
		 */
		QVariant defaultVal() const;
		/*!
		 * Returns the minimum option value.
		 * \note Only for options of type Spin.
		 */
		int min() const;
		/*!
		 * Returns the maximum option value.
		 * \note Only for options of type Spin.
		 */
		int max() const;
		/*!
		 * Returns a list of supported option values.
		 * \note Only for options of type Combo.
		 */
		const QStringList& var() const;
		
		/*! Returns true if the option is free of errors. */
		bool isOk() const;
		/*! Returns true if \a value is valid for this option. */
		bool isValueOk(const QVariant& value) const;
		
		/*! Sets the option name. */
		void setName(const QString& name);
		/*! Sets the option type. */
		void setType(Type type);
		/*! Sets the options's default value. */
		void setDefaultVal(const QVariant& defaultVal);
		/*! Sets the option's minimum value. */
		void setMin(int min);
		/*! Sets the option's maximum value. */
		void setMax(int max);
		/*! Adds a new Combo option value. */
		void addVar(const QString& str);
	
	private:
		QString m_name;
		Type m_type;
		QVariant m_defaultVal;
		int m_min;
		int m_max;
		QStringList m_var;
};

#endif // UCI_OPTION_H
