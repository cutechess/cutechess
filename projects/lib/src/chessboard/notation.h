#ifndef NOTATION_H
#define NOTATION_H

#include <QChar>
#include <QString>
#include "chessboard.h"

/*!
 * \brief Functions for converting between chess notations and low-level formats.
 *
 * All the notation conversion functions that don't require
 * a Chess::Board object should be placed here.
 */
namespace Notation
{
	/*! Converts a chess piece from an integer (Chess::Piece) to a character. */
	QChar pieceChar(int pieceCode);
	
	/*! Converts a chess piece from a character to an integer (Chess::Piece). */
	int pieceCode(const QChar& pieceChar);
	
	/*! Converts a Chess::Square object to a string. */
	QString squareString(const Chess::Square& square);
	
	/*! Converts a string to a Chess::Square object. */
	Chess::Square square(const QString& str);
}

#endif // NOTATION
