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

#ifndef PGNGAME_H
#define PGNGAME_H

#include <QMap>
#include <QString>
#include <QVector>
#include <QList>
#include <QPair>
#include <QDate>
#include <climits>
#include "board/genericmove.h"
#include "board/result.h"
class QTextStream;
class PgnStream;
class EcoNode;
class QObject;
namespace Chess { class Board; }


/*!
 * \brief A game of chess in PGN format.
 *
 * PGN (Portable game notation) is a text format for chess games.
 * Specification: http://www.very-best.de/pgn-spec.htm
 *
 * PgnGame is a middle format between text-based PGN games and games
 * played by Cute Chess. PgnGame objects are used for converting played
 * games to PGN format, importing PGN data to opening books, analyzing
 * previous games on a graphical board, etc.
 *
 * \sa PgnStream
 * \sa ChessGame
 */
class LIB_EXPORT PgnGame
{
	public:
		/*! The mode for writing PGN games. */
		enum PgnMode
		{
			//! Only use data which is required by the PGN standard
			Minimal,
			//! Use additional data like extra tags and comments
			Verbose
		};

		/*! \brief A struct for storing the game's move history. */
		struct MoveData
		{
			/*! The zobrist position key before the move. */
			quint64 key;
			/*! The move in the "generic" format. */
			Chess::GenericMove move;
			/*! The move in Standard Algebraic Notation. */
			QString moveString;
			/*! A comment/annotation describing the move. */
			QString comment;
		};

		/*! Creates a new PgnGame object. */
		PgnGame();
		/*! Returns true if the game doesn't contain any tags or moves. */
		bool isNull() const;
		/*! Deletes all tags and moves. */
		void clear();

		/*! Returns the tags that are used to describe the game. */
		QList< QPair<QString, QString> > tags() const;
		/*! Returns the moves that were played in the game. */
		const QVector<MoveData>& moves() const;
		/*! Adds a new move to the game.
		 * \param data The move to add.
		 * \param addEco Adds opening information if true.
		 */
		void addMove(const MoveData& data, bool addEco = true);
		void setMove(int ply, const MoveData& data);

		/*!
		 * Creates a board object for viewing or analyzing the game.
		 *
		 * The board is set to the game's starting position.
		 * Returns 0 on error.
		 */
		Chess::Board* createBoard() const;

		/*!
		 * Reads a game from a PGN text stream.
		 *
		 * \param in The PGN stream to read from.
		 * \param maxMoves The maximum number of halfmoves to read.
		 * \param addEco Adds opening information if true.
		 *
		 * \note Even if the stream contains multiple games,
		 * only one will be read.
		 *
		 * Returns true if any tags and/or moves were read.
		 */
		bool read(PgnStream& in, int maxMoves = INT_MAX - 1,
				  bool addEco = true);
		/*!
		 * Writes the game to a text stream.
		 *
		 * Returns true if successful; otherwise returns false.
		 */
		bool write(QTextStream& out, PgnMode mode = Verbose) const;
		/*!
		 * Writes the game to a file.
		 * If the file already exists, the game will be appended
		 * to the end of the file.
		 *
		 * Returns true if successful; otherwise returns false.
		 */
		bool write(const QString& filename, PgnMode mode = Verbose) const;
		
		/*!
		 * Returns true if the game's variant is "standard" and it's
		 * played from the default starting position; otherwise
		 * returns false.
		 */
		bool isStandard() const;
		/*!
		 * Returns the value of tag \a tag.
		 * If \a tag doesn't exist, an empty string is returned.
		 */
		QString tagValue(const QString& tag) const;
		/*! Returns the name of the tournament or match event. */
		QString event() const;
		/*! Returns the location of the event. */
		QString site() const;
		/*! Returns the starting date of the game. */
		QDate date() const;
		/*! Returns the the playing round ordinal of the game. */
		int round() const;
		/*! Returns the player's name who plays \a side. */
		QString playerName(Chess::Side side) const;
		/*! Returns the result of the game. */
		Chess::Result result() const;
		/*! Returns the chess variant of the game. */
		QString variant() const;
		/*! Returns the side that starts the game. */
		Chess::Side startingSide() const;
		/*! Returns the starting position's FEN string. */
		QString startingFenString() const;

		/*!
		 * Sets \a tag's value to \a value.
		 * If \a tag doesn't exist, a new tag is created.
		 */
		void setTag(const QString& tag, const QString& value);
		/*! Sets the name of the tournament or match event. */
		void setEvent(const QString& event);
		/*! Sets the location of the event. */
		void setSite(const QString& site);
		/*! Sets the starting date of the game. */
		void setDate(const QDate& date);
		/*! Sets the playing round ordinal of the game. */
		void setRound(int round);
		/*! Sets the player's name who plays \a side. */
		void setPlayerName(Chess::Side side, const QString& name);
		/*! Sets the result of the game. */
		void setResult(const Chess::Result& result);
		/*! Sets the chess variant of the game. */
		void setVariant(const QString& variant);
		/*! Sets the side that starts the game. */
		void setStartingSide(Chess::Side side);
		/*! Sets the starting position's FEN string. */
		void setStartingFenString(Chess::Side side, const QString& fen);
		/*!
		 * Sets a description for the result.
		 *
		 * The description is appended to the last move's comment/annotation.
		 * \note This is not the same as the "Termination" tag which can
		 *       only hold one of the standardized values.
		 */
		void setResultDescription(const QString& description);

		/*!
		 * Sets a receiver for PGN tags
		 *
		 * \a receiver is an object whose "setTag(QString tag, QString value)"
		 * slot is called when a PGN tag changes.
		 */
		void setTagReceiver(QObject* receiver);
		/*! Sets the starting time of the game */
		void setGameStartTime(const QDateTime& dateTime);
		/*! Sets the end time and the duration of the game */
		void setGameEndTime(const QDateTime& dateTime);

		/*! Returns QMap of scores extracted from PGN comments */
		QMap<int, int> extractScores() const;

	private:
		bool parseMove(PgnStream& in, bool addEco);
		
		Chess::Side m_startingSide;
		const EcoNode* m_eco;
		QMap<QString, QString> m_tags;
		QVector<MoveData> m_moves;
		QObject* m_tagReceiver;
		QString m_initialComment;
		static QString timeStamp(const QDateTime& dateTime);
		QDateTime m_gameStartTime;
};

/*! Reads a PGN game from a PGN stream. */
extern LIB_EXPORT PgnStream& operator>>(PgnStream& in, PgnGame& game);

/*! Writes a PGN game in verbose mode to a text stream. */
extern LIB_EXPORT QTextStream& operator<<(QTextStream& out, const PgnGame& game);

#endif // PGNGAME_H
