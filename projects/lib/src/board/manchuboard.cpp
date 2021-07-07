#include "manchuboard.h"
#include "easternzobrist.h"

namespace Chess {
ManchuBoard::ManchuBoard()
 : EasternBoard(new EasternZobrist())
{
    setPieceType(Banner,  tr("Banner"), "M", CannonMovement|KnightMovement|RookMovement, "XQR");
}

Board *ManchuBoard::copy() const
{
    return new ManchuBoard(*this);
}

QString ManchuBoard::variant() const
{
    return "manchu";
}

QString ManchuBoard::defaultFenString() const
{
    return "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/9/9/M1BAKAB2 w - - 0 1";
}

int ManchuBoard::width() const
{
    return 9;
}

int ManchuBoard::height() const
{
    return 10;
}

bool ManchuBoard::hasCastling() const
{
    return false;
}

}
