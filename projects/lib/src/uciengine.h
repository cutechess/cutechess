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
		/*! Creates a new UciEngine. */
		UciEngine(QIODevice* ioDevice, QObject* parent = 0);

		// Inherited from ChessEngine
		void start();
		void applySettings(const EngineSettings& settings);
		void endGame(Chess::Result result);
		void go(const Chess::Move& move);
		void makeMove(const Chess::Move& move);
		Protocol protocol() const;
		void ping(PingType type);
		void setConcurrency(int limit);
		void setEgbbPath(const QString& path);
		void setEgtbPath(const QString& path);
		
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
		
	protected:
		// Inherited from ChessEngine
		void startGame();
		void parseLine(const QString& line);
		void stopThinking();
		
	private:
		struct OptionCmd
		{
			QString name;
			QVariant value;
		};

		void addVariants();
		void parseInfo(const QString& line);
		void sendPosition();
		const UciOption* getOption(const QString& name) const;
		bool hasOption(const QString& name) const;
		
		bool m_isThinking;
		QString m_startFen;
		QString m_moveStrings;
		QVector<UciOption> m_options;
		QVector<OptionCmd> m_optionBuffer;
};

#endif // UCIENGINE_H
