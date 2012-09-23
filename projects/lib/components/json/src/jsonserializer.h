/*
    Copyright (c) 2010 Ilari Pihlajisto

    Permission is hereby granted, free of charge, to any person
    obtaining a copy of this software and associated documentation
    files (the "Software"), to deal in the Software without
    restriction, including without limitation the rights to use,
    copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following
    conditions:

    The above copyright notice and this permission notice shall be
    included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
    OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef JSONSERIALIZER_H
#define JSONSERIALIZER_H

#include <QString>
#include <QVariant>
#include <QCoreApplication>

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
	Q_DECLARE_TR_FUNCTIONS(JsonSerializer)

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
		bool hasError() const;
		/*! Returns a detailed description of the error. */
		QString errorString() const;

	private:
		bool serializeNode(QTextStream& stream,
				   const QVariant& node,
				   int indentLevel);
		void setError(const QString& message);

		bool m_error;
		const QVariant m_data;
		QString m_errorString;
};

#endif // JSONSERIALIZER_H
