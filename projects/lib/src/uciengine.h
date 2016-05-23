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

#include "chessengine.h"
#include <QVarLengthArray>


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
		UciEngine(QObject* parent = nullptr);

		// Inherited from ChessEngine
		virtual void endGame(const Chess::Result& result);
		virtual void makeMove(const Chess::Move& move);
		virtual QString protocol() const;
		virtual void startPondering();

	protected:
		// Inherited from ChessEngine
		virtual bool sendPing();
		virtual void sendStop();
		virtual void sendQuit();
		virtual void startProtocol();
		virtual void startGame();
		virtual void startThinking();
		virtual void parseLine(const QString& line);
		virtual void sendOption(const QString& name, const QVariant& value);
		virtual bool isPondering() const;
		
	private:
		enum PonderState
		{
			NotPondering,
			Pondering,
			PonderHit
		};

		static QStringRef parseUciTokens(const QStringRef& first,
						 const QString* types,
						 int typeCount,
						 QVarLengthArray<QStringRef>& tokens,
						 int& type);
		void parseInfo(const QVarLengthArray<QStringRef>& tokens,
			       int type);
		void parseInfo(const QStringRef& line);
		EngineOption* parseOption(const QStringRef& line);
		QString positionString();
		void sendPosition();
		void setPonderMove(const QString& moveString);
		
		QString m_variantOption;
		QString m_startFen;
		QString m_moveStrings;
		// Write buffer for messages that will be flushed to the engine
		// after it sends a "bestmove"
		QStringList m_bmBuffer;
		bool m_sendOpponentsName;
		bool m_canPonder;
		PonderState m_ponderState;
		Chess::Move m_ponderMove;
		bool m_ignoreThinking;
		bool m_rePing;
};

#endif // UCIENGINE_H
