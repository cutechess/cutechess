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

#include "humanplayer.h"
#include "board/board.h"

HumanPlayer::HumanPlayer(QObject* parent)
	: ChessPlayer(parent)
{
	setState(Idle);
	setName("Human");
}

void HumanPlayer::startGame()
{
	Q_ASSERT(m_bufferMove.isNull());
}

void HumanPlayer::startThinking()
{
	if (m_bufferMove.isNull())
		return;

	Chess::Move move(board()->moveFromGenericMove(m_bufferMove));
	m_bufferMove = Chess::GenericMove();

	if (board()->isLegalMove(move))
		emitMove(move);
}

void HumanPlayer::endGame(const Chess::Result& result)
{
	Q_ASSERT(m_bufferMove.isNull());

	ChessPlayer::endGame(result);
	setState(Idle);
}

void HumanPlayer::makeMove(const Chess::Move& move)
{
	Q_UNUSED(move);
	Q_ASSERT(m_bufferMove.isNull());
}

bool HumanPlayer::supportsVariant(const QString& variant) const
{
	Q_UNUSED(variant);
	return true;
}

bool HumanPlayer::isHuman() const
{
	return true;
}

void HumanPlayer::onHumanMove(const Chess::GenericMove& move,
			      const Chess::Side& side)
{
	if (side != this->side())
		return;

	Q_ASSERT(m_bufferMove.isNull());
	if (state() != Thinking)
	{
		if (state() == Observing)
			m_bufferMove = move;

		emit wokeUp();
		return;
	}

	Chess::Move tmp(board()->moveFromGenericMove(move));
	Q_ASSERT(board()->isLegalMove(tmp));

	emitMove(tmp);
}
