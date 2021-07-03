#include "minixiangqiboard.h"
#include "easternzobrist.h"

namespace Chess {
MiniXiangqiBoard::MiniXiangqiBoard():
    EasternBoard(new EasternZobrist()),
    m_sign(1),
    m_pawnAmbiguous(false),
    m_multiDigitNotation(false)
{
    setPieceType(Bishop, tr("no-Bishop"), "", 0);
    setPieceType(Advisor, tr("no-Advisor"), "", 0);
    setPieceType(Soldier, tr("Soldier"), "P", SoldierMovement, "XQP");
    setPieceType(General, tr("General"), "K", GeneralMovement, "XQK");
}

Board* MiniXiangqiBoard::copy() const
{
    return new MiniXiangqiBoard(*this);
}

QString MiniXiangqiBoard::variant() const
{
    return "minixiangqi";
}

QString MiniXiangqiBoard::defaultFenString() const
{
    return "rcnkncr/p1ppp1p/7/7/7/P1PPP1P/RCNKNCR w - - 0 1";
}

int MiniXiangqiBoard::width() const
{
    return 7;
}

int MiniXiangqiBoard::height() const
{
    return 7;
}

bool MiniXiangqiBoard::hasCastling() const
{
    return false;
}

bool MiniXiangqiBoard::pawnHasDoubleStep() const
{
    return false;
}

bool MiniXiangqiBoard::inCheck(Side side, int square) const
{
    if (square == 0)
	    return false;
    Side opSide = side.opposite();
    Piece oppKing(opSide, General);
    Piece ownKing(side, General);
    Piece piece;

    // Soldier attacks
    QVarLengthArray<int> pawnAttackOffsets;
    pawnAttackOffsets.clear();
    int arrWidth = width() + 2;
    int p_sign = (side == Side::Black) ? 1: -1;
    int attackStep = p_sign * arrWidth;

    //pawnAttackOffsets.resize(3); // the code in this line can cause crash
    pawnAttackOffsets.append(attackStep);
    pawnAttackOffsets.append(m_soldierPreOffsets[1]);
    pawnAttackOffsets.append(m_soldierPreOffsets[2]);
    for (int i = 0; i < pawnAttackOffsets.size(); i++)
    {
        piece = pieceAt(square + pawnAttackOffsets[i]);
        if (piece.side() == opSide
        &&  pieceHasMovement(piece.type(), SoldierMovement))
            return true;
    }

    // General meet Attack (in the basic minixiangqi rule, the two opposite
    // General are not allowed to face to face
    for (int i = 0; i < m_generalMeetOffsets.size(); i++)
    {
        int offset = m_generalMeetOffsets[i];
        int targetSquare = square + offset;

        Piece capture;
        int obstacle = 0;
        while(!(capture = pieceAt(targetSquare)).isWall())
        {
            if(!capture.isEmpty())
            {
                obstacle++;
                if(obstacle == 1 && capture.side() == opSide
                   && capture.type() == General)
                {
                    return true;
                }
            }
            targetSquare += offset;
        }
    }

    return EasternBoard::inCheck(side, square);
}

bool MiniXiangqiBoard::inPalace(int square) const
{
    bool retval = false;
    //return true;
    Square sq = chessSquare(square);
    if(((sq.file() > 1) && (sq.file() < 5)) &&
      (((sq.rank() >= 0) && (sq.rank() < 3)) ||
       ((sq.rank() > 3) && (sq.rank() < 7))))
    {
        retval = true;
    }
    else
    {
        retval = false;
    }

    return retval;
}

void MiniXiangqiBoard::vInitialize()
{

    EasternBoard::vInitialize();

    int arrWidth = width() + 2;

    m_soldierPreOffsets.resize(3);
    m_soldierPreOffsets[0] = arrWidth;
    m_soldierPreOffsets[1] = -1;
    m_soldierPreOffsets[2] = 1;

    m_crossOffsets.resize(4);
    m_crossOffsets[0] = -arrWidth;
    m_crossOffsets[1] = -1;
    m_crossOffsets[2] = 1;
    m_crossOffsets[3] = arrWidth;

    m_generalMeetOffsets.resize(2);
    m_generalMeetOffsets[0] = -arrWidth;
    m_generalMeetOffsets[1] = arrWidth;

    m_multiDigitNotation =  (height() > 9 && coordinateSystem() == NormalCoordinates)
            || (width() > 9 && coordinateSystem() == InvertedCoordinates);
}

void MiniXiangqiBoard::generateMovesForPiece(QVarLengthArray<Move> &moves, int pieceType, int square) const
{
    Chess::EasternBoard::generateMovesForPiece(moves, pieceType, square);
    if(pieceHasMovement(pieceType, SoldierMovement))
        generateSoldierMoves(square, moves);
    if(pieceHasMovement(pieceType, GeneralMovement))
        generateGeneralMoves(square, moves);
}

void MiniXiangqiBoard::generateGeneralMoves(int sourceSquare, QVarLengthArray<Move> &moves) const
{
    QVarLengthArray<int> kingRelOffsets;
    kingRelOffsets.clear();
    for(int i = 0; i < m_crossOffsets.size(); i++)
    {
        int targetSquare = sourceSquare + m_crossOffsets[i];
        if(inPalace(targetSquare))
        {
            //moves.append(Move(sourceSquare, targetSquare));
            kingRelOffsets.append(m_crossOffsets[i]);
        }
    }
    generateHoppingMoves(sourceSquare, kingRelOffsets, moves);
}

void MiniXiangqiBoard::generateSoldierMoves(int sourceSquare, QVarLengthArray<Move> &moves) const
{
    QVarLengthArray<int> soldierRelOffsets;
    soldierRelOffsets.clear();
    int arrWidth = width() + 2;

    Side side(sideToMove());
    int p_sign = (side == Side::Black) ? 1: -1;
    int step = p_sign * arrWidth;

    soldierRelOffsets.append(step);
    soldierRelOffsets.append(m_soldierPreOffsets[1]);
    soldierRelOffsets.append(m_soldierPreOffsets[2]);

    generateHoppingMoves(sourceSquare, soldierRelOffsets, moves);
}

} // namespace Chess
