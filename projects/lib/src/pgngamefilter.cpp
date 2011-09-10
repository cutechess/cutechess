#include "pgngamefilter.h"
#include <pgngameentry.h>


PgnGameFilter::PgnGameFilter()
	: m_type(Advanced),
	  m_minRound(0),
	  m_maxRound(0),
	  m_result(AnyResult)
{
}

PgnGameFilter::PgnGameFilter(const QString& pattern)
	: m_type(FixedString),
	  m_pattern(pattern.toLatin1()),
	  m_minRound(0),
	  m_maxRound(0),
	  m_result(AnyResult)
{
}

void PgnGameFilter::setPattern(const QString& pattern)
{
	m_type = FixedString;
	m_pattern = pattern.toLatin1();
}

void PgnGameFilter::setEvent(const QString& event)
{
	m_event = event.toLatin1();
}

void PgnGameFilter::setSite(const QString& site)
{
	m_site = site.toLatin1();
}

void PgnGameFilter::setPlayer(const QString& name, Chess::Side side)
{
	m_player = name.toLatin1();
	m_playerSide = side;
}

void PgnGameFilter::setOpponent(const QString& name)
{
	m_opponent = name.toLatin1();
}

void PgnGameFilter::setMinDate(const QDate& date)
{
	m_minDate = date;
}

void PgnGameFilter::setMaxDate(const QDate& date)
{
	m_maxDate = date;
}

void PgnGameFilter::setMinRound(int round)
{
	m_minRound = round;
}

void PgnGameFilter::setMaxRound(int round)
{
	m_maxRound = round;
}

void PgnGameFilter::setResult(Result result)
{
	m_result = result;
}
