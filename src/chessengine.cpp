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
#include <QIODevice>

#include "chessengine.h"
#include "chessboard/chessboard.h"

int ChessEngine::m_count = 0;

ChessEngine::ChessEngine(QIODevice* ioDevice,
                         Chessboard* chessboard,
                         TimeControl* timeControl,
                         QObject* parent)
: ChessPlayer(timeControl, parent)
{
	Q_CHECK_PTR(ioDevice);
	Q_CHECK_PTR(chessboard);
	m_isReady = false;
	m_notation = LongNotation;
	m_ioDevice = ioDevice;
	m_id = m_count++;
	m_chessboard = chessboard;
	QObject::connect(m_ioDevice, SIGNAL(readyRead()),
	                 this, SLOT(on_readyRead()));
}

ChessEngine::~ChessEngine()
{
	--m_count;
}

bool ChessEngine::isHuman() const
{
	return false;
}

bool ChessEngine::isReady() const
{
	return m_isReady;
}

void ChessEngine::write(const QString& data) const
{
	Q_ASSERT(m_ioDevice->isWritable());
	emit debugMessage(QString(">") + name() + "(" + QString::number(m_id) +  "): " + data);

	m_ioDevice->write(data.toAscii() + "\n");
}

void ChessEngine::on_readyRead()
{
	while (m_ioDevice->canReadLine())
	{
		QString line = QString(m_ioDevice->readLine());
		line.chop(1); // remove the trailing newline
		emit debugMessage(QString("<") + name() + "(" + QString::number(m_id) +  "): " + line);
		
		parseLine(line);
	}
}

