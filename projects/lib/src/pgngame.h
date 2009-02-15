#ifndef PGNGAME_H
#define PGNGAME_H

#include <QString>
#include <QVector>
#include <QDate>
#include "chessboard/chess.h"
#include "chessboard/result.h"
#include "chessboard/variant.h"
#include "chessboard/chessmove.h"
class ChessGame;
class PgnFile;


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
class LIB_EXPORT PgnGame
{
	public:
		/*! Constructs a PgnGame from a ChessGame object. */
		explicit PgnGame(const ChessGame* game);
		
		/*!
		 * Constructs a PgnGame from a text stream.
		 *
		 * \param in The input PGN file.
		 * \param maxMoves The maximum number of halfmoves to read.
		 * \note Even if the stream contains multiple games,
		 * only one will be read.
		 */
		explicit PgnGame(PgnFile& in, int maxMoves = 1000);
		
		/*!
		 * Write the game to a file.
		 * If the file already exists, the game will be appended
		 * to the end of the file.
		 */
		void write(const QString& filename) const;
		
		/*! Returns true if the game doesn't contain any moves. */
		bool isEmpty() const;
		
		/*! Returns the starting position's FEN string. */
		QString startingFen() const;
		
		/*! Returns the moves that were played in the game. */
		const QVector<Chess::Move>& moves() const;
	
	private:
		enum PgnItem
		{
			PgnMove,
			PgnMoveNumber,
			PgnTag,
			PgnComment,
			PgnNag,
			PgnResult,
			PgnEmpty,
			PgnError
		};

		PgnItem readItem(PgnFile& in);
		
		QVector<Chess::Move> m_moves;
		QString m_whitePlayer;
		QString m_blackPlayer;
		QString m_event;
		QString m_site;
		QString m_fen;
		Chess::Variant m_variant;
		bool m_isRandomVariant;
		bool m_hasTags;
		Chess::Result m_result;
		int m_round;
};

#endif // PGNGAME_H

