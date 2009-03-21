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

#include <QString>
#include <QIODevice>
#include <QtDebug>

#include "chessengine.h"
#include "chessboard/chess.h"
#include "enginesettings.h"

int ChessEngine::m_count = 0;

ChessEngine::ChessEngine(QIODevice* ioDevice, QObject* parent)
	: ChessPlayer(parent),
	  m_notation(Chess::LongAlgebraic),
	  m_initialized(false),
	  m_finishingGame(false),
	  m_id(m_count++),
	  m_pingType(PingUnknown),
	  m_ioDevice(ioDevice)
{
	Q_ASSERT(m_ioDevice != 0);
	Q_ASSERT(m_ioDevice->isOpen());
	
	connect(m_ioDevice, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onDisconnect()));

	m_pingTimer.setSingleShot(true);
	connect(&m_pingTimer, SIGNAL(timeout()), this, SLOT(onPingTimeout()));
}

ChessEngine::~ChessEngine()
{
	--m_count;
}

void ChessEngine::applySettings(const EngineSettings& settings)
{
	if (settings.concurrency() > 0)
		setConcurrency(settings.concurrency());
	if (!settings.egbbPath().isEmpty())
		setEgbbPath(settings.egbbPath());
	if (!settings.egtbPath().isEmpty())
		setEgbbPath(settings.egtbPath());
	if (settings.timeControl().isValid())
		setTimeControl(settings.timeControl());
}

void ChessEngine::endGame(Chess::Result result)
{
	ChessPlayer::endGame(result);

	m_finishingGame = true;
	if (m_isReady)
		ping(PingUnknown);
}

bool ChessEngine::isHuman() const
{
	return false;
}

void ChessEngine::onDisconnect()
{
	m_pingTimer.stop();
	m_timer.stop();
	m_isReady = true;
	m_writeBuffer.clear();
	m_pingType = PingUnknown;
	emit ready();

	ChessPlayer::onDisconnect();
}

void ChessEngine::ping(PingType type)
{
	Q_UNUSED(type);
	m_finishingGame = false;
	m_pingTimer.start(10000);
}

void ChessEngine::pong()
{
	if (m_isReady)
		return;

	m_pingTimer.stop();
	m_isReady = true;
	flushWriteBuffer();
	if (m_pingType == PingMove)
		startClock();
	m_pingType = PingUnknown;

	if (m_finishingGame)
		ping(PingUnknown);
	else
		emit ready();
}

void ChessEngine::onPingTimeout()
{
	qDebug() << "Engine" << m_name << "failed to respond to ping";

	m_isReady = true;
	m_writeBuffer.clear();
	m_pingType = PingUnknown;

	Chess::Result result(Chess::Result::WinByStalledConnection, otherSide());
	emit forfeit(result);
}

void ChessEngine::write(const QString& data)
{
	if (!m_isReady)
	{
		m_writeBuffer.append(data);
		return;
	}

	Q_ASSERT(m_ioDevice->isWritable());
	emit debugMessage(QString(">") + name() + "(" + QString::number(m_id) +  "): " + data);

	m_ioDevice->write(data.toAscii() + "\n");
}

void ChessEngine::onReadyRead()
{
	while (m_ioDevice->isReadable() && m_ioDevice->canReadLine())
	{
		QString line = QString(m_ioDevice->readLine()).trimmed();
		emit debugMessage(QString("<") + name() + "(" + QString::number(m_id) +  "): " + line);
		
		parseLine(line.simplified());
	}
}

void ChessEngine::flushWriteBuffer()
{
	Q_ASSERT(m_isReady);

	foreach (const QString& line, m_writeBuffer)
		write(line);
	m_writeBuffer.clear();
}

void ChessEngine::quit()
{
	if (!m_ioDevice->isOpen())
		return;

	disconnect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onDisconnect()));
	write("quit");
}
