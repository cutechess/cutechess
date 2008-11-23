#ifndef PGNGAME_H
#define PGNGAME_H

#include <QString>
#include <QStringList>
#include <QDate>
#include "chessboard/chess.h"
class ChessGame;


/*!
 * \brief A class for reading and writing chess games in PGN format.
 *
 * PGN (Portable game notation) is a text format for chess games of any
 * variants: http://en.wikipedia.org/wiki/Portable_Game_Notation
 *
 * The PgnGame class has a method for reading games, which can then be
 * viewed on the graphical board, analyzed by chess engines, added to
 * an opening book, etc.
 * PgnGame can also take a ChessGame object and convert it into PGN format.
 */
class PgnGame
{
	public:
		/*! Constructs a PgnGame from a ChessGame object. */
		PgnGame(const ChessGame* game);
		
		/*!
		 * Constructs a PgnGame from a PGN file.
		 * \note If the PGN file contains multiple games, only
		 * the first one will be read.
		 */
		PgnGame(const QString& filename);
		
		/*!
		 * Write the game to a file.
		 * If the file already exists, the game will be appended
		 * to the end of the file.
		 */
		void write(const QString& filename) const;
	
	private:
		QStringList m_moves;
		QString m_whitePlayer;
		QString m_blackPlayer;
		QString m_event;
		QString m_site;
		QString m_fen;
		Chess::Variant m_variant;
		bool m_isRandomVariant;
		Chess::Result m_result;
		int m_round;
};

#endif // PGNGAME_H
