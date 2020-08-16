/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2020 Cute Chess authors

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

#include "shakoboard.h"
#include "westernzobrist.h"

namespace Chess {

ShakoBoard::ShakoBoard()
    : WesternBoard(new WesternZobrist())
{
    setPieceType(Cannon,   tr("cannon"),   "C", CannonMovement);
    setPieceType(Elephant, tr("elephant"), "E", BiSqDiagMovement);
}

Board* ShakoBoard::copy() const
{
    return new ShakoBoard(*this);
}

QString ShakoBoard::variant() const
{
    return "shako";
}

int ShakoBoard::height() const
{
    return 10;
}

int ShakoBoard::width() const
{
    return 10;
}

QString ShakoBoard::defaultFenString() const
{
    return "c8c/ernbqkbnre/pppppppppp/10/10/10/10/PPPPPPPPPP/ERNBQKBNRE/C8C w KQkq - 0 1";
}

inline int ShakoBoard::pawnPushOffset(const PawnStep& ps, int sign) const
{
	return sign * ps.file - sign * (width() + 2) * 1;
}

void ShakoBoard::generateMovesForPiece(QVarLengthArray< Move >& moves,
				       int pieceType,
				       int square) const
{
    if (pieceHasMovement(pieceType, BiSqDiagMovement))
        generateHoppingMoves(square, m_biSquareDiagOffsets, moves);
    if (pieceHasMovement(pieceType, CannonMovement))
        generateCannonMoves(square, moves);
    if (pieceType == Pawn)
        generatePawnMoves(square, moves);
    if (pieceType != Pawn)
        WesternBoard::generateMovesForPiece(moves, pieceType, square);
}

void ShakoBoard::vInitialize()
{
    WesternBoard::vInitialize();
    int arrWidth = width() + 2;

    m_rookOffsets.resize(4);
    m_rookOffsets[0] = -arrWidth;
    m_rookOffsets[1] = -1;
    m_rookOffsets[2] = 1;
    m_rookOffsets[3] = arrWidth;

    m_biSquareDiagOffsets.resize(8);
    m_biSquareDiagOffsets[0] = -arrWidth - 1;
    m_biSquareDiagOffsets[1] = -arrWidth + 1;
    m_biSquareDiagOffsets[2] =  arrWidth - 1;
    m_biSquareDiagOffsets[3] =  arrWidth + 1;
    m_biSquareDiagOffsets[4] = -2 * arrWidth - 2;
    m_biSquareDiagOffsets[5] = -2 * arrWidth + 2;
    m_biSquareDiagOffsets[6] =  2 * arrWidth + 2;
    m_biSquareDiagOffsets[7] =  2 * arrWidth - 2;
}

bool ShakoBoard::inCheck(Side side, int square) const
{
    Side oppSide = side.opposite();
    Piece piece;

    if (square == 0)
        square = kingSquare(side);

    // Cannons attacks
    for (int i = 0; i < m_rookOffsets.size(); i++)
    {
        int offset = m_rookOffsets[i];
        int checkSquare = square + offset;
        int obstacle = 0;

        while (!(piece = pieceAt(checkSquare)).isWall())
        {
            if (!piece.isEmpty())
            {
                obstacle++;
                if (obstacle == 2 && piece.side() == oppSide
                &&  pieceHasMovement(piece.type(), CannonMovement))
                    return true;
            }
            checkSquare += offset;
        }
    }

    // Elephant attacks
    for (int i = 0; i < m_biSquareDiagOffsets.size(); i++)
    {
        piece = pieceAt(square + m_biSquareDiagOffsets[i]);

        if (!piece.isEmpty())
        {
            if (piece.side() == oppSide
            &&  pieceHasMovement(piece.type(), BiSqDiagMovement))
                return true;
        }
    }

    return WesternBoard::inCheck(side, square);
}

void ShakoBoard::addPromotions(int sourceSquare, int targetSquare, QVarLengthArray<Move> &moves) const
{
    WesternBoard::addPromotions(sourceSquare, targetSquare, moves);
    moves.append(Move(sourceSquare, targetSquare, Cannon));
    moves.append(Move(sourceSquare, targetSquare, Elephant));
}

void ShakoBoard::generatePawnMoves(int sourceSquare, QVarLengthArray<Move> &moves) const
{
    WesternBoard::generateMovesForPiece(moves, Pawn, sourceSquare);

    Side side = sideToMove();
    int sign = (side == Side::White) ? 1 : -1;
    int rank = chessSquare(sourceSquare).rank();

    // add missing pawn double steps from third rank
    int rank3 = (side == Side::White) ? 2 : height() - 3;
    if (rank == rank3)
    {
        for (const PawnStep& pStep: m_pawnSteps)
        {
            if (pStep.type != FreeStep)
                continue;

            int targetSquare = sourceSquare + pawnPushOffset(pStep, sign);
            Piece capture = pieceAt(targetSquare);

            if (capture.isEmpty())
            {
                targetSquare += pawnPushOffset(pStep, sign);
                capture = pieceAt(targetSquare);
                if (capture.isEmpty())
                    moves.append(Move(sourceSquare, targetSquare));
            }
        }
    }
}

int ShakoBoard::castlingFile(WesternBoard::CastlingSide castlingSide) const
{
    Q_ASSERT(castlingSide != NoCastlingSide);
    return castlingSide == QueenSide ? 3 : width() - 3; // usually D and H
}

int ShakoBoard::castlingRank(Chess::Side castlingSide) const
{
    Q_ASSERT(castlingSide != Side::NoSide);
    return castlingSide == Side::Black ? 3 : height(); // usually 2 and 9
}

void ShakoBoard::generateCannonMoves(int sourceSquare, QVarLengthArray<Move> &moves) const
{
    QVarLengthArray<int> cannonRelOffsets;
    cannonRelOffsets.clear();
    Side side = sideToMove();
    for (int i = 0; i < m_rookOffsets.size(); i++)
    {
        int offset = m_rookOffsets[i];
        int targetSquare = sourceSquare + offset;

        Piece capture;
        int obstacle = 0;
        while (!(capture = pieceAt(targetSquare)).isWall())
        {
            if(capture.isEmpty())
            {
                if(obstacle == 0)
                    cannonRelOffsets.append(targetSquare - sourceSquare);
            }
            else
            {
                obstacle++;
                if(obstacle == 2 && capture.side() == side.opposite())
                {
                    cannonRelOffsets.append(targetSquare - sourceSquare);
                    break;
                }
            }
            targetSquare += offset;
        }
    }

    generateHoppingMoves(sourceSquare, cannonRelOffsets, moves);
}

} // namespace Chess
