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

#ifndef UCIENGINE_H
#define UCIENGINE_H

#include <QString>
#include "chessengine.h"
#include "ucioption.h"

/*!
 * \brief A chess engine which uses the UCI chess interface.
 *
 * UCI's specifications: http://wbec-ridderkerk.nl/html/UCIProtocol.html
 */
class LIB_EXPORT UciEngine : public ChessEngine
{
	Q_OBJECT

	public:
		/*!
		 * Creates a new UciEngine object.
		 *
		 * \param ioDevice An open chess engine process or socket.
		 * \param parent The parent object.
		 */
		UciEngine(QIODevice* ioDevice, QObject* parent = 0);

		/*! Destroys the engine. */
		~UciEngine();

		// Inherited methods
		void newGame(Chess::Side side, ChessPlayer* opponent);
		void endGame(Chess::Result result);
		void go();
		void makeMove(const Chess::Move& move);
		Protocol protocol() const;
		void ping(PingType type);
		
		/*!
		 * Sets \a option to \a value, and sends it to the engine.
		 *
		 * \note If the option is invalid, or the engine doesn't
		 * have the option, nothing happens.
		 */
		void setOption(const UciOption* option, const QVariant& value);
		/*!
		 * Sets an option with the name \a name to \a value.
		 *
		 * \note If the engine doesn't have an option called \a name,
		 * nothing happens.
		 */
		void setOption(const QString& name, const QVariant& value);
		
		void setConcurrency(int limit);
		void setEgbbPath(const QString& path);
		void setEgtbPath(const QString& path);

	protected:
		void parseLine(const QString& line);
		
	private:
		void addVariants();
		void sendPosition();
		const UciOption* getOption(const QString& name) const;
		bool hasOption(const QString& name) const;
		
		QString m_startFen;
		QStringList m_moves;
		QVector<UciOption> m_options;
};

#endif // UCIENGINE_H
