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
class PgnStream;


/*!
 * \brief A class for reading and writing chess games in PGN format.
 *
 * PGN (Portable game notation) is a text format for chess games of any
 * variants: http://en.wikipedia.org/wiki/Portable_Game_Notation
 *
 * \sa PgnStream
 * \sa ChessGame
 */
class LIB_EXPORT PgnGame
{
	public:
		/*! The mode for reading and writing PGN games. */
		enum PgnMode
		{
			//! Only use data which is required by the PGN standard
			Minimal,
			//! Use additional data like extra tags and comments
			Verbose
		};

		/*! A struct for storing the game's move history. */
		struct MoveData
		{
			/*! The move in the internal format. */
			Chess::Move move;
			/*! The move in Standard Algebraic notation. */
			QString sanMove;
			/*! A comment or annotation for the move. */
			QString comment;
		};

		/*! Creates a new PgnGame object. */
		explicit PgnGame(Chess::Variant variant = Chess::Variant::Standard);
		
		/*!
		 * Reads a game from a PGN text stream.
		 *
		 * \param in The input PGN file.
		 * \param mode The PGN mode for reading the game.
		 * \param maxMoves The maximum number of halfmoves to read.
		 *
		 * \note Even if the stream contains multiple games,
		 * only one will be read.
		 *
		 * \return True, if a FEN tag or moves were read.
		 */
		bool read(PgnStream& in, PgnMode mode = Verbose, int maxMoves = 1000);
		
		/*!
		 * Write the game to a file.
		 * If the file already exists, the game will be appended
		 * to the end of the file.
		 *
		 * \return True if successfull
		 */
		bool write(const QString& filename, PgnMode mode = Verbose) const;
		
		/*! Returns true if the game doesn't contain any moves. */
		bool isEmpty() const;
		
		/*! Returns the event/tournament name. */
		QString event() const;

		/*! Returns the site/location where the game was played. */
		QString site() const;

		/*! Returns the round number of a match or tournament. */
		int round() const;

		/*! Returns the player's name who plays \a side. */
		QString playerName(Chess::Side side) const;

		/*! Sets a player's name. */
		void setPlayerName(Chess::Side side, const QString& name);

		/*! Returns the starting position's FEN string. */
		QString startingFen() const;

		/*! Returns the game result. */
		Chess::Result result() const;

		/*!
		 * Returns the time control for \a side.
		 *
		 * \note Usually both players have the same time control, in which
		 * case the default value for \a side should be used.
		 */
		const TimeControl& timeControl(Chess::Side side = Chess::White) const;

		/*! Returns the moves that were played in the game. */
		const QVector<MoveData>& moves() const;

		/*! Sets the event (eg. "My tournament"). */
		void setEvent(const QString& event);

		/*! Sets the site (eg. "My basement"). */
		void setSite(const QString& site);

		/*! Sets the round number in a match or tournament. */
		void setRound(int round);

		/*! Sets the game result. */
		void setResult(const Chess::Result& result);

		/*! Sets the starting position's FEN string to \a fen. */
		void setStartingFen(const QString& fen);

		/*!
		 * Sets the time control for \a side to \a timeControl.
		 *
		 * \note If \a side is Chess::NoSide (the default), the same
		 * time control will be used for both players.
		 */
		void setTimeControl(const TimeControl& timeControl,
				    Chess::Side side = Chess::NoSide);

	protected:
		/*!
		 * Adds a new move to the game.
		 *
		 * \param move The move in the internal format.
		 * \param board The board object that is used to convert the
		 * move to SAN.
		 * \param comment A comment/annotation for the move.
		 *
		 * \return True if the move is legal.
		 */
		bool addMove(const Chess::Move& move,
			     Chess::Board* board,
			     const QString& comment = QString());

		/*!
		 * Adds a new move to the game.
		 *
		 * \param moveString The move in Standard Algebraic notation.
		 * \param board The board object that is used to convert the
		 * move to the internal format.
		 * \param comment A comment/annotation for the move.
		 *
		 * \return True if the move is legal.
		 */
		bool addMove(const QString& moveString,
			     Chess::Board* board,
			     const QString& comment = QString());

		/*! The game's moves. */
		QVector<MoveData> m_moves;
		
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

		PgnItem readItem(PgnStream& in, PgnMode mode);
		
		bool m_hasTags;
		int m_round;
		QString m_event;
		QString m_site;
		QString m_fen;
		QString m_playerName[2];
		Chess::Side m_startingSide;
		Chess::Result m_result;
		Chess::Variant m_variant;
		TimeControl m_timeControl[2];
};

#endif // PGNGAME_H
