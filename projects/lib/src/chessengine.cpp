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
	  m_whiteEvalPov(false),
	  m_id(m_count++),
	  m_pingState(NotStarted),
	  m_pinging(false),
	  m_ioDevice(ioDevice)
{
	Q_ASSERT(m_ioDevice != 0);
	
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
	foreach (const QString& str, settings.initStrings())
		write(str);

	if (settings.concurrency() > 0)
		setConcurrency(settings.concurrency());
	if (!settings.egbbPath().isEmpty())
		setEgbbPath(settings.egbbPath());
	if (!settings.egtbPath().isEmpty())
		setEgbbPath(settings.egtbPath());
	if (settings.timeControl().isValid())
		setTimeControl(settings.timeControl());
	m_whiteEvalPov = settings.whiteEvalPov();
}

void ChessEngine::start()
{
	if (state() != NotStarted)
		return;
	
	m_pinging = false;
	setState(Starting);

	flushWriteBuffer();
	
	startProtocol();
	m_pinging = true;
}

void ChessEngine::onProtocolStart()
{
	m_pinging = false;
	setState(Idle);
	Q_ASSERT(isReady());
}

void ChessEngine::go()
{
	if (state() == Idle)
		ping();
	ChessPlayer::go();
}

void ChessEngine::endGame(Chess::Result result)
{
	ChessPlayer::endGame(result);
	ping();
}

bool ChessEngine::isHuman() const
{
	return false;
}

bool ChessEngine::isReady() const
{
	if (m_pinging)
		return false;
	return ChessPlayer::isReady();
}

bool ChessEngine::supportsVariant(Chess::Variant variant) const
{
	return m_variants.contains(variant);
}

void ChessEngine::closeConnection()
{
	ChessPlayer::closeConnection();

	m_pinging = false;
	m_pingTimer.stop();
	m_writeBuffer.clear();
	emit ready();

	disconnect(m_ioDevice, SIGNAL(readChannelFinished()),
		   this, SLOT(onDisconnect()));
	m_ioDevice->close();
}

void ChessEngine::onTimeout()
{
	stopThinking();
}

void ChessEngine::ping()
{
	if (m_pinging || state() == NotStarted || !sendPing())
		return;

	m_pinging = true;
	m_pingState = state();
	m_pingTimer.start(10000);
}

void ChessEngine::pong()
{
	if (!m_pinging)
		return;

	m_pingTimer.stop();
	m_pinging = false;
	flushWriteBuffer();

	if (state() == FinishingGame)
	{
		if (m_pingState == FinishingGame)
		{
			setState(Idle);
			m_pingState = Idle;
		}
		// If the status changed while waiting for a ping response, then
		// ping again to make sure that we can move on to the next game.
		else
		{
			ping();
			return;
		}
	}

	emit ready();
}

void ChessEngine::onPingTimeout()
{
	qDebug() << "Engine" << name() << "failed to respond to ping";

	m_pinging = false;
	m_writeBuffer.clear();
	closeConnection();

	emitForfeit(Chess::Result::WinByStalledConnection);
}

void ChessEngine::write(const QString& data)
{
	if (state() == Disconnected)
		return;
	if (state() == NotStarted || m_pinging)
	{
		m_writeBuffer.append(data);
		return;
	}

	Q_ASSERT(m_ioDevice->isWritable());
	emit debugMessage(QString(">%1(%2): %3")
			  .arg(name())
			  .arg(m_id)
			  .arg(data));

	m_ioDevice->write(data.toAscii() + "\n");
}

void ChessEngine::onReadyRead()
{
	while (m_ioDevice->isReadable() && m_ioDevice->canReadLine())
	{
		QString line = QString(m_ioDevice->readLine()).simplified();
		emit debugMessage(QString("<%1(%2): %3")
				  .arg(name())
				  .arg(m_id)
				  .arg(line));
		parseLine(line);
	}
}

void ChessEngine::flushWriteBuffer()
{
	if (m_pinging || state() == NotStarted)
		return;

	foreach (const QString& line, m_writeBuffer)
		write(line);
	m_writeBuffer.clear();
}

void ChessEngine::quit()
{
	if (!m_ioDevice->isOpen() || !isConnected())
		return;

	disconnect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onDisconnect()));
	sendQuit();
	setState(Disconnected);
}
