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

#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include <QString>
#include <QVariant>

class QTextStream;


/*!
 * \brief A JSON (JavaScript Object Notation) serializer.
 *
 * JsonSerializer converts QVariants into JSON data and writes it
 * to a text stream. The following QVariant types are supported:
 * - QVariant::Invalid (JSON null)
 * - QVariant::Bool (JSON boolean)
 * - QVariant::ByteArray (JSON string)
 * - QVariant::List (JSON array)
 * - QVariant::Map (JSON object)
 * - QVariant::String (JSON string)
 * - QVariant::StringList (JSON array)
 * - any other type that can be converted into a string by QVariant
 *
 * JSON specification: http://json.org/
 * \sa JsonParser
 */
class LIB_EXPORT JsonSerializer
{
	public:
		/*! Creates a new serializer that operates on \a data. */
		JsonSerializer(const QVariant& data);
		/*!
		 * Converts the data into JSON format and writes it to
		 * \a stream.
		 *
		 * Returns false if an invalid or unsupported variant type
		 * is encountered. Otherwise returns true.
		 */
		bool serialize(QTextStream& stream);

		/*! Returns true if an error occured. */
		bool isError() const;
		/*! Returns a detailed description of the error. */
		QString errorMessage() const;

	private:
		bool serializeNode(QTextStream& stream,
				   const QVariant& node,
				   int indentLevel);
		void setError(const QString& message);

		bool m_error;
		const QVariant m_data;
		QString m_errorMessage;
};

#endif // JSONSERIALIZER_H
