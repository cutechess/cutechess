#ifndef PGNGAME_H
#define PGNGAME_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QDate>
#include "chessboard/chess.h"
#include "chessboard/result.h"
#include "chessboard/variant.h"
#include "chessboard/chessmove.h"
#include "timecontrol.h"
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
		/*! Creates a new PgnGame object. */
		explicit PgnGame(Chess::Variant variant = Chess::Variant::Standard);
		
		/*!
		 * Reads a game from a PGN text stream.
		 *
		 * \param in The input PGN file.
		 * \param maxMoves The maximum number of halfmoves to read.
		 * \note Even if the stream contains multiple games,
		 * only one will be read.
		 */
		void load(PgnFile& in, int maxMoves = 1000);
		
		/*!
		 * Write the game to a file.
		 * If the file already exists, the game will be appended
		 * to the end of the file.
		 */
		void write(const QString& filename) const;
		
		/*! Returns true if the game doesn't contain any moves. */
		bool isEmpty() const;
		
		/*! Returns the player's name who plays \a side. */
		QString playerName(Chess::Side side) const;

		/*! Sets a player's name. */
		void setPlayerName(Chess::Side side, const QString& name);

		/*! Returns the starting position's FEN string. */
		QString startingFen() const;

		/*! Returns the game result. */
		Chess::Result result() const;

		/*! Returns the moves that were played in the game. */
		const QVector<Chess::Move>& moves() const;
		
		/*! Sets the event (eg. "My tournament"). */
		void setEvent(const QString& event);

		/*! Sets the site (eg. "My basement"). */
		void setSite(const QString& site);

		/*! Sets the round number in a match or tournament. */
		void setRound(int round);

	protected:
		QString m_fen;
		QVector<Chess::Move> m_moves;
		QStringList m_comments;
		Chess::Result m_result;
		Chess::Variant m_variant;
		TimeControl m_timeControl[2];
		bool m_hasTags;

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
		
		QString m_playerName[2];
		QString m_event;
		QString m_site;
		int m_round;
};

#endif // PGNGAME_H

