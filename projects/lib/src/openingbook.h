#ifndef OPENING_BOOK_H
#define OPENING_BOOK_H

#include <QtGlobal>
#include <QMultiMap>
#include "board/genericmove.h"

class QString;
class QDataStream;
class PgnGame;
class PgnStream;


/*!
 * \brief A collection of opening moves for chess.
 *
 * OpeningBook is a container (binary tree) class for opening moves that
 * can be played by the GUI. When the game goes "out of book", control
 * of the game is transferred to the players.
 *
 * The opening book can be stored externally in a binary file. When it's needed,
 * it is loaded in memory, and positions can be found quickly by searching
 * the book for Zobrist keys that match the current board position.
 */
class LIB_EXPORT OpeningBook
{
	public:
		/*! Destroys the opening book. */
		virtual ~OpeningBook() {}
		
		/*!
		 * Imports a PGN game.
		 *
		 * \param pgn The game to import.
		 * \param maxMoves The maximum number of halfmoves that
		 * can be imported.
		 *
		 * Returns the number of moves imported.
		 */
		int import(const PgnGame& pgn, int maxMoves);
		/*!
		 * Imports PGN games from a stream.
		 *
		 * \param in The PGN stream that contains the games.
		 * \param maxMoves The maximum number of halfmoves per game
		 * that can be imported.
		 *
		 * Returns the number of moves imported.
		 */
		int import(PgnStream& in, int maxMoves);
		
		/*!
		 * Returns a move that can be played in a position where the
		 * Zobrist key is \a key.
		 *
		 * If no matching moves are found, an empty (illegal) move is
		 * returned.
		 *
		 * If there are multiple matches, a random, weighted move is
		 * returned. Popular moves have a higher probablity of being
		 * selected than unpopular ones.
		 */
		Chess::GenericMove move(quint64 key) const;

		/*!
		 * Reads a book from \a filename.
		 * Returns true if successfull.
		 */
		bool read(const QString& filename);

		/*!
		 * Writes the book to \a filename.
		 * Returns true if successfull.
		 */
		bool write(const QString& filename) const;


	protected:
		friend LIB_EXPORT QDataStream& operator>>(QDataStream& in, OpeningBook* book);
		friend LIB_EXPORT QDataStream& operator<<(QDataStream& out, const OpeningBook* book);

		/*!
		 * \brief An entry in the opening book.
		 *
		 * \note Each entry is paired with a Zobrist key.
		 * \note The book file may not use the same structure
		 * for the entries.
		 */
		struct Entry
		{
			/*! A book move. */
			Chess::GenericMove move;
			/*!
			 * A weight or score, usually based on popularity
			 * of the move. The higher the weight, the more
			 * likely the move will be played.
			 */
			quint16 weight;
		};

		/*! The type of binary tree. */
		typedef QMultiMap<quint64, Entry> Map;


		/*! Adds a new entry to the book. */
		void addEntry(const Entry& entry, quint64 key);
		
		/*!
		 * Reads a new book entry from \a in.
		 *
		 * The implementation must call addEntry() to add the
		 * entry to the book.
		 */
		virtual void readEntry(QDataStream& in) = 0;
		
		/*! Writes the key and entry pointed to by \a it, to \a out. */
		virtual void writeEntry(const Map::const_iterator& it,
					QDataStream& out) const = 0;

	private:
		Map m_map;
};

/*!
 * Reads a book from a data stream.
 *
 * \note Multiple book files can be appended to the same
 * OpeningBook object.
 */
extern LIB_EXPORT QDataStream& operator>>(QDataStream& in, OpeningBook* book);

/*!
 * Writes a book to a data stream.
 *
 * \warning Do not write multiple OpeningBook objects to the same
 * data stream, because the books are likely to have duplicate
 * entries.
 */
extern LIB_EXPORT QDataStream& operator<<(QDataStream& out, const OpeningBook* book);

#endif // OPENING_BOOK_H
