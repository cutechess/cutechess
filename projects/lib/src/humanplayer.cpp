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

#include "humanplayer.h"
#include "board/board.h"


HumanPlayer::HumanPlayer(QObject* parent)
	: ChessPlayer(parent)
{
	setState(Idle);
	setName("HumanPlayer");
}

void HumanPlayer::startGame()
{
}

void HumanPlayer::startThinking()
{
}

void HumanPlayer::endGame(Chess::Result result)
{
	ChessPlayer::endGame(result);
	setState(Idle);
}

void HumanPlayer::makeMove(const Chess::Move& move)
{
	Q_UNUSED(move);
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

void HumanPlayer::onHumanMove(const Chess::GenericMove& move)
{
	if (state() != Thinking)
		return;

	Chess::Move boardMove = board()->moveFromGenericMove(move);

	QVector<Chess::Move> moves(board()->legalMoves());
	QList<int> promotions;
	foreach (const Chess::Move& tmp, moves)
	{
		if (tmp.sourceSquare() == boardMove.sourceSquare()
		&&  tmp.targetSquare() == boardMove.targetSquare())
		{
			int prom = tmp.promotion();

			// Make sure "No promotion" is at the top
			if (prom == Chess::Piece::NoPiece)
				promotions.prepend(prom);
			else
				promotions.append(prom);
		}
	}

	if (promotions.size() == 1)
	{
		Chess::Move tmp(boardMove.sourceSquare(),
				boardMove.targetSquare(),
				promotions.first());
		emitMove(tmp);
	}
	else if (promotions.size() > 1)
		emit needsPromotion(board(), boardMove, promotions);
}

void HumanPlayer::onPromotionMove(const Chess::Move& move)
{
	if (state() != Thinking)
		return;

	if (board()->isLegalMove(move))
		emitMove(move);
}
