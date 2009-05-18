#ifndef NOTATION_H
#define NOTATION_H

#include <QChar>
#include <QString>
namespace Chess { struct Square; }


/*!
 * \brief Functions for converting between chess notations and low-level formats.
 *
 * All the notation conversion functions that don't require
 * a Chess::Board object should be placed here.
 */
namespace Notation {

/*! Converts a Chess::Square object to a string. */
QString squareString(const Chess::Square& square);

/*! Converts a string to a Chess::Square object. */
Chess::Square square(const QString& str);

} // namespace Notation
#endif // NOTATION
