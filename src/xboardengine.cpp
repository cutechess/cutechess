/*
    This file is part of SloppyGUI.

    SloppyGUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    SloppyGUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SloppyGUI.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QString>
#include <QStringList>
#include <QDebug>

#include "xboardengine.h"
#include "chessboard/chessmove.h"


XboardEngine::XboardEngine(QIODevice* ioDevice, Chessboard* chessboard, QObject* parent)
: ChessEngine(ioDevice, chessboard, parent)
{
	setName(QString("XboardEngine ") + QString::number(m_id));
	// Tell the engine to turn on xboard mode
	write("xboard");
	// Tell the engine that we're using Xboard protocol 2
	write("protover 2");
}

XboardEngine::~XboardEngine()
{
	//write("quit");
}

void XboardEngine::newGame(Chessboard::ChessSide side, const QString& fen)
{
	setSide(side);
	write("force");
	write("setboard " + fen);
}

void XboardEngine::sendOpponentsMove(const ChessMove& move) const
{
	QString moveString;
	if (m_notation == LongNotation)
		moveString = m_chessboard->coordMoveString(move);
	else if (m_notation == StandardNotation)
		moveString = m_chessboard->sanMoveString(move);
	
	write(moveString);
}

void XboardEngine::go()
{
	write("go");
}

void XboardEngine::setTimeControl(TimeControl timeControl)
{

}

void XboardEngine::setTimeLeft(int timeLeft)
{

}

ChessEngine::ChessProtocol XboardEngine::protocol() const
{
	return ChessEngine::Xboard;
}

void XboardEngine::parseLine(const QString& line)
{
	QString command = line.section(' ', 0, 0);
	QString args = line.right(line.length() - command.length() - 1);

	if (command == "move") {
		ChessMove move = m_chessboard->chessMoveFromString(args);
		emit moveMade(move);
	} else if (command == "feature") {
		QRegExp rx("\\w+\\s*=\\s*(\"[^\"]*\"|\\d+)");

		int pos = 0;
		QString arg;
		QString feature;
		QStringList list;
		while ((pos = args.indexOf(rx, pos)) != -1) {
			arg = args.mid(pos, rx.matchedLength());
			pos += rx.matchedLength();
			
			list = arg.split('=');
			if (list.count() != 2)
				continue;
			feature = list[0].trimmed();
			if (feature == "san") {
				if (list[1].trimmed() == "1")
					m_notation = StandardNotation;
			} else if (feature == "myname") {
				m_name = list[1].trimmed();
				m_name.remove('\"');
			} else if (feature == "done") {
				if (list[1].trimmed() == "1")
					m_isReady = true;
			}
		}
	}
}

