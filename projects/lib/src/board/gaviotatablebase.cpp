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

#include "gaviotatablebase.h"
#include <QStringList>
#include <gtb-probe.h>
#include "westernboard.h"

const char** s_paths = nullptr;
char* s_initInfo = nullptr;


bool GaviotaTablebase::initialize(const QStringList& paths)
{
	s_paths = tbpaths_init();
	foreach (const QString& path, paths)
	{
		s_paths = tbpaths_add(s_paths, path.toLatin1().constData());
		if (s_paths == nullptr)
			return false;
	}

	s_initInfo = tb_init(1, tb_CP4, s_paths);
	tbcache_init(32*1024*1024, 96);
	tbstats_reset();

	return s_initInfo != nullptr;
}

void GaviotaTablebase::cleanup()
{
	if (s_initInfo == nullptr)
		return;

	tbcache_done();
	tb_done();
	s_paths = tbpaths_done(s_paths);
	s_initInfo = nullptr;
}

bool GaviotaTablebase::tbAvailable(int pieces)
{
	int bit = (pieces - 3) * 2;
	return tb_availability() & (1 << bit);
}

static TB_squares tbSquare(const Chess::Square& square)
{
	if (!square.isValid())
		return tb_NOSQUARE;
	return TB_squares(square.rank() * 8 + square.file());
}

static TB_pieces tbPiece(int pieceType)
{
	switch (pieceType)
	{
	case Chess::WesternBoard::Pawn:
		return tb_PAWN;
	case Chess::WesternBoard::Knight:
		return tb_KNIGHT;
	case Chess::WesternBoard::Bishop:
		return tb_BISHOP;
	case Chess::WesternBoard::Rook:
		return tb_ROOK;
	case Chess::WesternBoard::Queen:
		return tb_QUEEN;
	case Chess::WesternBoard::King:
		return tb_KING;
	default:
		return tb_NOPIECE;
	}
}

Chess::Result GaviotaTablebase::result(const Chess::Side& side,
				       const Chess::Square& enpassantSq,
				       Castling castling,
				       const PieceList& pieces,
				       unsigned int* dtm)
{
	Q_ASSERT(pieces.size() <= 5);

	if (s_initInfo == nullptr)
		return Chess::Result();

	TB_sides stm = (side == Chess::Side::White) ? tb_WHITE_TO_MOVE : tb_BLACK_TO_MOVE;
	TB_squares epsq = tbSquare(enpassantSq);
	unsigned int sq[2][17];
	unsigned char pc[2][17];

	typedef QPair<Chess::Square, Chess::Piece> PcSq;
	int pcIndex[2] = { 0, 0 };
	foreach (const PcSq& item, pieces)
	{
		Chess::Side pcSide = item.second.side();
		int i = pcIndex[pcSide]++;
		sq[pcSide][i] = tbSquare(item.first);
		pc[pcSide][i] = tbPiece(item.second.type());
	}
	sq[0][pcIndex[0]] = tb_NOSQUARE;
	sq[1][pcIndex[1]] = tb_NOSQUARE;
	pc[0][pcIndex[0]] = tb_NOPIECE;
	pc[1][pcIndex[1]] = tb_NOPIECE;

	unsigned info = tb_UNKNOWN;
	bool ok = false;
	if (dtm == nullptr)
		ok = tb_probe_WDL_hard(stm, epsq, castling,
				       sq[0], sq[1], pc[0], pc[1],
				       &info);
	else
		ok = tb_probe_hard(stm, epsq, castling,
				   sq[0], sq[1], pc[0], pc[1],
				   &info, dtm);

	if (ok)
	{
		Chess::Side winner(Chess::Side::NoSide);
		if (info == tb_WMATE)
			winner = Chess::Side::White;
		else if (info == tb_BMATE)
			winner = Chess::Side::Black;

		return Chess::Result(Chess::Result::Adjudication, winner, "GTB");
	}

	return Chess::Result();
}
