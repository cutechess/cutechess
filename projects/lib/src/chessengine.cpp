/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

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
#include <QSettings>

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
	  m_timeoutScale(1.0),
	  m_pingTimer(new QTimer(this)),
	  m_quitTimer(new QTimer(this)),
	  m_idleTimer(new QTimer(this)),
	  m_protocolStartTimer(new QTimer(this)),
	  m_ioDevice(nullptr),
	  m_restartMode(EngineConfiguration::RestartAuto)
{
	m_pingTimer->setSingleShot(true);
	m_pingTimer->setInterval(defaultPingTimeout);
	connect(m_pingTimer, SIGNAL(timeout()), this, SLOT(onPingTimeout()));

	m_quitTimer->setSingleShot(true);
	m_quitTimer->setInterval(defaultQuitTimeout);
	connect(m_quitTimer, SIGNAL(timeout()), this, SLOT(onQuitTimeout()));

	m_idleTimer->setSingleShot(true);
	m_idleTimer->setInterval(defaultIdleTimeout);
	connect(m_idleTimer, SIGNAL(timeout()), this, SLOT(onIdleTimeout()));

	m_protocolStartTimer->setSingleShot(true);
	m_protocolStartTimer->setInterval(defaultProtocolStartTimeout);
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

	const auto initStrings = configuration.initStrings();
	for (const QString& str : initStrings)
		write(str);

	const auto options = configuration.options();
	for (const auto option : options)
	{
		if (option->isEditable())
			setOption(option->name(), option->value());
	}

	m_whiteEvalPov = configuration.whiteEvalPov();
	m_pondering = configuration.pondering();
	m_timeoutScale = configuration.timeoutScale();
	m_restartMode = configuration.restartMode();
	setClaimsValidated(configuration.areClaimsValidated());

	// Read protocol timeouts from QSettings.
	// Scale timer intervals according to m_timeoutScale
	QSettings settings;
	settings.beginGroup("Timing");

	int pingTimeout = m_timeoutScale * settings.value("PingTimeout", defaultPingTimeout).toInt();
	int quitTimeout = m_timeoutScale * settings.value("QuitTimeout", defaultQuitTimeout).toInt();
	int idleTimeout = m_timeoutScale * settings.value("IdleTimeout", defaultIdleTimeout).toInt();
	double protocolStartTimeout = m_timeoutScale * settings.value("ProtocolStartTimeout",
								      defaultProtocolStartTimeout).toInt();
	settings.endGroup();

	m_pingTimer->setInterval(pingTimeout);
	m_quitTimer->setInterval(quitTimeout);
	m_idleTimer->setInterval(idleTimeout);
	m_protocolStartTimer->setInterval(protocolStartTimeout);
}

void ChessEngine::addOption(EngineOption* option)
{
	Q_ASSERT(option != nullptr);
	m_options.append(option);
}

EngineOption* ChessEngine::getOption(const QString& name) const
{
	for (EngineOption* option : qAsConst(m_options))
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
		qWarning("%s doesn't have option %s",
			 qUtf8Printable(this->name()),
			 qUtf8Printable(name));
		return;
	}

	if (!option->isValid(value))
	{
		qWarning("Invalid value for option %s: %s",
			 qUtf8Printable(name),
			 qUtf8Printable(value.toString()));
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

int ChessEngine::id() const
{
	return m_id;
}

bool ChessEngine::stopThinking()
{
	if (state() == Thinking || isPondering())
	{
		if (!m_pinging)
		{
			m_idleTimer->start();
			sendStop();
			return true;
		}
		// A bit of a hack: if the engine is set to thinking or
		// pondering state and is being pinged, we can assume that
		// whatever is in the write buffer is obsolete by now because
		// the engine is being told to stop.
		m_writeBuffer.clear();
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

	qInfo("Terminating process of engine %s(%d)",
	      qUtf8Printable(name()), m_id);

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
	setError(tr("no response to ping"));
	qWarning("Engine %s(%d): %s",
	         qUtf8Printable(name()), m_id,
	         qUtf8Printable(errorString()));

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
		qWarning("Writing to engine %s(%d) failed",
			 qUtf8Printable(name()), m_id);
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

	for (const QString& line : qAsConst(m_writeBuffer))
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

	setError(tr("Chess protocol was not started in time"));
	qWarning("Engine %s(%d): %s",
	         qUtf8Printable(name()), m_id,
	         qUtf8Printable(errorString()));
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
	{
		ChessPlayer::quit();
		return;
	}
	disconnect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onCrashed()));
	connect(m_ioDevice, SIGNAL(readChannelFinished()), this, SLOT(onQuitTimeout()));
	sendQuit();
	m_quitTimer->start();
}
