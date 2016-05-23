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

#include "chessengine.h"
#include <QIODevice>
#include <QTimer>
#include <QStringRef>
#include <QtAlgorithms>
#include "engineoption.h"


int ChessEngine::s_count = 0;

QStringRef ChessEngine::nextToken(const QStringRef& previous, bool untilEnd)
{
	const QString* str = previous.string();
	if (str == nullptr)
		return QStringRef();

	int i;
	int start = -1;
	int firstPos = previous.position() + previous.size();

	for (i = firstPos; i < str->size(); i++)
	{
		if (str->at(i).isSpace())
		{
			if (start == -1)
				continue;
			break;
		}
		else if (start == -1)
		{
			start = i;
			if (untilEnd)
			{
				int end = str->size();
				while (str->at(--end).isSpace())
					;
				i = end + 1;
				break;
			}
		}
	}

	if (start == -1)
		return QStringRef();
	return QStringRef(str, start, i - start);
}

QStringRef ChessEngine::firstToken(const QString& str, bool untilEnd)
{
	return nextToken(QStringRef(&str, 0, 0), untilEnd);
}


ChessEngine::ChessEngine(QObject* parent)
	: ChessPlayer(parent),
	  m_id(s_count++),
	  m_pingState(NotStarted),
	  m_pinging(false),
	  m_whiteEvalPov(false),
	  m_pondering(false),
	  m_pingTimer(new QTimer(this)),
	  m_quitTimer(new QTimer(this)),
	  m_idleTimer(new QTimer(this)),
	  m_protocolStartTimer(new QTimer(this)),
	  m_ioDevice(nullptr),
	  m_restartMode(EngineConfiguration::RestartAuto)
{
	m_pingTimer->setSingleShot(true);
	m_pingTimer->setInterval(10000);
	connect(m_pingTimer, SIGNAL(timeout()), this, SLOT(onPingTimeout()));

	m_quitTimer->setSingleShot(true);
	m_quitTimer->setInterval(5000);
	connect(m_quitTimer, SIGNAL(timeout()), this, SLOT(onQuitTimeout()));

	m_idleTimer->setSingleShot(true);
	m_idleTimer->setInterval(10000);
	connect(m_idleTimer, SIGNAL(timeout()), this, SLOT(onIdleTimeout()));

	m_protocolStartTimer->setSingleShot(true);
	m_protocolStartTimer->setInterval(35000);
	connect(m_protocolStartTimer, SIGNAL(timeout()),
		this, SLOT(onProtocolStartTimeout()));
}

ChessEngine::~ChessEngine()
{
	qDeleteAll(m_options);
}

QIODevice* ChessEngine::device() const
{
	return m_ioDevice;
}

void ChessEngine::setDevice(QIODevice* device)
{
	Q_ASSERT(device != nullptr);

	m_ioDevice = device;
	m_ioDevice->setParent(this);

	connect(m_ioDevice, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onCrashed()));
}

void ChessEngine::applyConfiguration(const EngineConfiguration& configuration)
{
	if (!configuration.name().isEmpty())
		setName(configuration.name());

	foreach (const QString& str, configuration.initStrings())
		write(str);

	foreach (EngineOption* option, configuration.options())
		setOption(option->name(), option->value());

	m_whiteEvalPov = configuration.whiteEvalPov();
	m_pondering = configuration.pondering();
	m_restartMode = configuration.restartMode();
	setClaimsValidated(configuration.areClaimsValidated());
}

void ChessEngine::addOption(EngineOption* option)
{
	Q_ASSERT(option != nullptr);
	m_options.append(option);
}

EngineOption* ChessEngine::getOption(const QString& name) const
{
	foreach (EngineOption* option, m_options)
	{
		if (option->alias() == name || option->name() == name)
			return option;
	}

	return nullptr;
}

void ChessEngine::setOption(const QString& name, const QVariant& value)
{
	if (state() == Starting || state() == NotStarted)
	{
		m_optionBuffer[name] = value;
		return;
	}

	EngineOption* option = getOption(name);
	if (option == nullptr)
	{
		qDebug("%s doesn't have option %s", qPrintable(this->name()), qPrintable(name));
		return;
	}

	if (!option->isValid(value))
	{
		qDebug("Invalid value for option %s: %s", qPrintable(name),
			qPrintable(value.toString()));
		return;
	}

	option->setValue(value);
	sendOption(option->name(), option->value());
}

QList<EngineOption*> ChessEngine::options() const
{
	return m_options;
}

QStringList ChessEngine::variants() const
{
	return m_variants;
}

void ChessEngine::addVariant(const QString& variant)
{
	if (!m_variants.contains(variant))
		m_variants << variant;
}

void ChessEngine::clearVariants()
{
	m_variants.clear();
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
	m_protocolStartTimer->start();
}

void ChessEngine::onProtocolStart()
{
	m_protocolStartTimer->stop();
	m_pinging = false;
	setState(Idle);
	Q_ASSERT(isReady());

	flushWriteBuffer();

	QMap<QString, QVariant>::const_iterator i = m_optionBuffer.constBegin();
	while (i != m_optionBuffer.constEnd())
	{
		setOption(i.key(), i.value());
		++i;
	}
	m_optionBuffer.clear();
}

void ChessEngine::go()
{
	if (state() == Observing && !isPondering())
		ping();
	ChessPlayer::go();
}

EngineConfiguration::RestartMode ChessEngine::restartMode() const
{
	return m_restartMode;
}

bool ChessEngine::restartsBetweenGames() const
{
	return m_restartMode == EngineConfiguration::RestartOn;
}

bool ChessEngine::isPondering() const
{
	return false;
}

bool ChessEngine::whiteEvalPov() const
{
	return m_whiteEvalPov;
}

bool ChessEngine::pondering() const
{
	return m_pondering;
}

void ChessEngine::endGame(const Chess::Result& result)
{
	ChessPlayer::endGame(result);

	if (restartsBetweenGames())
		quit();
	else
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

bool ChessEngine::supportsVariant(const QString& variant) const
{
	return m_variants.contains(variant);
}

bool ChessEngine::stopThinking()
{
	if ((state() == Thinking || isPondering()) && !m_pinging)
	{
		m_idleTimer->start();
		sendStop();
		return true;
	}
	return false;
}

void ChessEngine::onIdleTimeout()
{
	m_idleTimer->stop();
	if (state() != Thinking || m_pinging)
		return;

	m_writeBuffer.clear();
	kill();

	forfeit(Chess::Result::StalledConnection);
}

void ChessEngine::kill()
{
	if (state() == Disconnected)
		return;

	qDebug("Terminating process of engine %s(%d)",
	       qPrintable(name()), m_id);

	m_pinging = false;
	m_pingTimer->stop();
	m_protocolStartTimer->stop();
	m_writeBuffer.clear();

	disconnect(m_ioDevice, SIGNAL(readChannelFinished()),
		   this, SLOT(onCrashed()));
	m_ioDevice->close();

	ChessPlayer::kill();
}

void ChessEngine::onTimeout()
{
	stopThinking();
}

void ChessEngine::ping(bool sendCommand)
{
	if (m_pinging
	||  state() == NotStarted
	||  state() == Disconnected
	||  (sendCommand && !sendPing()))
		return;

	m_pinging = true;
	m_pingState = state();
	m_pingTimer->start();
}

void ChessEngine::pong(bool emitReady)
{
	if (!m_pinging)
		return;

	m_pingTimer->stop();
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

	if (emitReady)
		emit ready();
}

void ChessEngine::onPingTimeout()
{
	qDebug("Engine %s(%d) failed to respond to ping",
	       qPrintable(name()), m_id);

	m_pinging = false;
	m_writeBuffer.clear();
	kill();

	forfeit(Chess::Result::StalledConnection);
}

void ChessEngine::write(const QString& data, WriteMode mode)
{
	if (state() == Disconnected)
		return;
	if (state() == NotStarted
	||  (m_pinging && mode == Buffered))
	{
		m_writeBuffer.append(data);
		return;
	}

	Q_ASSERT(m_ioDevice->isWritable());
	emit debugMessage(QString(">%1(%2): %3")
			  .arg(name())
			  .arg(m_id)
			  .arg(data));

	if (m_ioDevice->write(data.toLatin1() + "\n") == -1)
		qDebug("Writing to engine %s(%d) failed",
		       qPrintable(name()), m_id);
}

void ChessEngine::onReadyRead()
{
	while (m_ioDevice->isReadable() && m_ioDevice->canReadLine())
	{
		QString line = QString(m_ioDevice->readLine());
		if (line.endsWith('\n'))
			line.chop(1);
		if (line.endsWith('\r'))
			line.chop(1);
		if (line.isEmpty())
			continue;

		emit debugMessage(QString("<%1(%2): %3")
				  .arg(name())
				  .arg(m_id)
				  .arg(line));
		parseLine(line);

		if (m_idleTimer->isActive())
		{
			if (state() == Thinking && !m_pinging)
				m_idleTimer->start();
			else
				m_idleTimer->stop();
		}
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

void ChessEngine::clearWriteBuffer()
{
	m_writeBuffer.clear();
}

void ChessEngine::onProtocolStartTimeout()
{
	if (state() != Starting)
		return;

	qDebug("Engine %s(%d) did not start the chess protocol in time",
	       qPrintable(name()), m_id);
	onCrashed();
}

void ChessEngine::onQuitTimeout()
{
	Q_ASSERT(state() != Disconnected);

	disconnect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onQuitTimeout()));

	if (!m_quitTimer->isActive())
		kill();
	else
		m_quitTimer->stop();

	ChessPlayer::quit();
}

void ChessEngine::quit()
{
	if (!m_ioDevice || !m_ioDevice->isOpen() || state() == Disconnected)
		return ChessPlayer::quit();

	disconnect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onCrashed()));
	connect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onQuitTimeout()));
	sendQuit();
	m_quitTimer->start();
}
