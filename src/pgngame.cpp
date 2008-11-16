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

#include <QFile>
#include <QTextStream>
#include <QDate>
#include "pgngame.h"
#include "chessgame.h"
#include "chessboard/chessboard.h"
#include "chessplayer.h"


PgnGame::PgnGame(const ChessGame* game)
{
	Q_ASSERT(game != 0);
	
	m_whitePlayer = game->whitePlayer()->name();
	m_blackPlayer = game->blackPlayer()->name();
	Chess::Board* board = game->chessboard();
	m_moves = board->moveStringList(Chess::StandardAlgebraic);
	m_fen = board->startingFen();
	m_variant = board->variant();
	m_isRandomVariant = board->isRandomVariant();
	m_result = game->result();
}

void PgnGame::write(const QString& filename) const
{
	bool useFen = false;
	QString variantString;
	if (m_variant == Chess::StandardChess)
	{
		if (m_fen != Chess::standardFen)
			useFen = true;
		if (m_isRandomVariant)
			variantString = "Fischerandom";
	}
	else if (m_variant == Chess::CapablancaChess)
	{
		if (m_fen == Chess::capablancaFen)
			variantString = "Capablanca";
		else if (m_fen == Chess::gothicFen)
			variantString = "Gothic";
		else
			useFen = true;
		if (m_isRandomVariant)
			variantString = "Capablancarandom";
	}
	
	QString resultString;
	switch (m_result)
	{
	case Chess::WhiteMates: case Chess::BlackResigns:
		resultString = "1-0";
		break;
	case Chess::BlackMates: case Chess::WhiteResigns:
		resultString = "0-1";
		break;
	case Chess::Stalemate: case Chess::DrawByMaterial:
	case Chess::DrawByRepetition: case Chess::DrawByFiftyMoves:
	case Chess::DrawByAgreement:
		resultString = "1/2-1/2";
		break;
	default:
		resultString = "*";
		break;
	};
	
	QString date = QDate::currentDate().toString("yyyy.MM.dd");
	
	QFile file(filename);
	if (file.open(QIODevice::Append))
	{
		QTextStream out(&file);
		
		out << "[Date \"" << date << "\"]\n";
		out << "[White \"" << m_whitePlayer << "\"]\n";
		out << "[Black \"" << m_blackPlayer << "\"]\n";
		out << "[Result \"" << resultString << "\"]\n";
		if (!variantString.isEmpty())
			out << "[Variant \"" << variantString << "\"]\n";
		if (useFen)
			out << "[FEN \"" << m_fen << "\"]\n";
		
		out << "\n";
		for (int i = 0; i < m_moves.size(); i++)
		{
			if ((i % 2) == 0)
				out << QString::number(i / 2 + 1) << ". ";
			out << m_moves[i] << " ";
		}
		out << resultString << "\n\n";
	}
}
