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

#include "syzygytablebase.h"
#include <QDir>
#include <QMutex>
#include <QStringList>
#include <tbprobe.h>
#include "westernboard.h"

namespace {

bool s_initialized = false, s_initOK = false, s_noRule50 = false;
int s_pieces = INT_MAX;
QMutex s_mutex;

int tbSquare(const Chess::Square& square)
{
	if (!square.isValid())
		return -1;
	return square.rank() * 8 + square.file();
}

} // anonymous namespace

bool SyzygyTablebase::initialize(const QString& path)
{
	if (s_initialized)
		return s_initOK;
	s_initialized = true;

	const auto nativePath = QDir::toNativeSeparators(path);
	s_initOK = tb_init(nativePath.toStdString().c_str());

	return s_initOK;
}

bool SyzygyTablebase::tbAvailable(int pieces)
{
	return s_initOK && ((unsigned)pieces <= TB_LARGEST);
}

void SyzygyTablebase::setPieces(int pieces)
{
	if (pieces > 2)
		s_pieces = pieces;
}

void SyzygyTablebase::setNoRule50()
{
	s_noRule50 = true;
}

Chess::Result SyzygyTablebase::result(const Chess::Side& side,
					   const Chess::Square& enpassantSq,
					   Castling castling,
					   int rule50,
					   const PieceList& pieces,
					   unsigned int* dtz)
{
	if (!s_initOK)
		return Chess::Result();
	if (castling)
		return Chess::Result();
	if (pieces.size() > s_pieces)
		return Chess::Result();

	bool wtm = (side == Chess::Side::White);
	unsigned ep = (tbSquare(enpassantSq) < 0? 0: tbSquare(enpassantSq));
	uint64_t white = 0, black = 0;
	uint64_t kings = 0, queens = 0, rooks = 0, bishops = 0, knights = 0,
		pawns = 0;
	typedef QPair<Chess::Square, Chess::Piece> PcSq;
	for (const PcSq& item : pieces)
	{
		if (tbSquare(item.first) < 0)
			continue;
		unsigned sq = tbSquare(item.first);
		uint64_t bit = ((uint64_t)1 << sq);
		if (item.second.side() == Chess::Side::White)
			white |= bit;
		else
			black |= bit;
		switch (item.second.type())
		{
		case Chess::WesternBoard::Pawn:
			pawns |= bit; break;
		case Chess::WesternBoard::Knight:
			knights |= bit; break;
		case Chess::WesternBoard::Bishop:
			bishops |= bit; break;
		case Chess::WesternBoard::Rook:
			rooks |= bit; break;
		case Chess::WesternBoard::Queen:
			queens |= bit; break;
		case Chess::WesternBoard::King:
			kings |= bit; break;
		}
	}

	s_mutex.lock();
	unsigned result = tb_probe_root(white, black, kings, queens, rooks,
		bishops, knights, pawns, rule50, 0, ep, wtm, nullptr);
	s_mutex.unlock();

	Chess::Side winner(Chess::Side::NoSide); 
	if (result == TB_RESULT_FAILED)
		return Chess::Result();
	if (result == TB_RESULT_CHECKMATE)
		winner = (wtm? Chess::Side::Black: Chess::Side::White);
	else if (result == TB_RESULT_STALEMATE)
		winner = Chess::Side::NoSide;
	else
	{
		switch (TB_GET_WDL(result))
		{
		case TB_BLESSED_LOSS:
			if (!s_noRule50)
				break;
			// Fallthrough
		case TB_LOSS:
			winner = (wtm? Chess::Side::Black: Chess::Side::White);
			break;
		case TB_DRAW:
			break;
		case TB_CURSED_WIN:
			if (!s_noRule50)
				break;
			// Fallthrough
		case TB_WIN:
			winner = (wtm? Chess::Side::White: Chess::Side::Black);
			break;
		}
	}
	if (dtz != nullptr)
		*dtz = TB_GET_DTZ(result);
	return Chess::Result(Chess::Result::Adjudication, winner, "SyzygyTB");
}
