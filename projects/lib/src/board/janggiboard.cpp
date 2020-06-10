#include "janggiboard.h"
#include "easternzobrist.h"

namespace Chess {
JanggiBoard::JanggiBoard():
    KoreanBoard(new EasternZobrist())
{
}

Board *JanggiBoard::copy() const
{
    return new JanggiBoard(*this);
}

QString JanggiBoard::variant() const
{
    return "janggi";
}

QString JanggiBoard::defaultFenString() const
{
    return "rnba1abnr/4k4/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/4K4/RNBA1ABNR w - - 0 1";
}

bool JanggiBoard::hasBikjang() const
{
    return true;
}

bool JanggiBoard::hasPieceCount() const
{
    return true;
}

} // namespace Chess

namespace Chess {

JanggiCasualBoard::JanggiCasualBoard():
    KoreanBoard(new EasternZobrist())
{
}

Board *JanggiCasualBoard::copy() const
{
    return new JanggiCasualBoard(*this);
}

QString JanggiCasualBoard::variant() const
{
    return "janggicasual";
}

QString JanggiCasualBoard::defaultFenString() const
{
    return "rnba1abnr/4k4/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/4K4/RNBA1ABNR w - - 0 1";
}

bool JanggiCasualBoard::hasBikjang() const
{
    return false;
}

bool JanggiCasualBoard::hasPieceCount() const
{
    return false;
}

} // namespace Chess

namespace Chess {
JanggiModernBoard::JanggiModernBoard():
    KoreanBoard(new EasternZobrist())
{
}

Board *JanggiModernBoard::copy() const
{
    return new JanggiModernBoard(*this);
}

QString JanggiModernBoard::variant() const
{
    return "janggimodern";
}

QString JanggiModernBoard::defaultFenString() const
{
    return "rnba1abnr/4k4/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/4K4/RNBA1ABNR w - - 0 1";
}

bool JanggiModernBoard::hasBikjang() const
{
    return false;
}

bool JanggiModernBoard::hasPieceCount() const
{
    return true;
}

} // namespace Chess


namespace Chess {
JanggiTraditionalBoard::JanggiTraditionalBoard():
    KoreanBoard(new EasternZobrist())
{
}

Board *JanggiTraditionalBoard::copy() const
{
    return new JanggiTraditionalBoard(*this);
}

QString JanggiTraditionalBoard::variant() const
{
    return "janggitraditional";
}

QString JanggiTraditionalBoard::defaultFenString() const
{
    return "rnba1abnr/4k4/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/4K4/RNBA1ABNR w - - 0 1";
}

bool JanggiTraditionalBoard::hasBikjang() const
{
    return true;
}

bool JanggiTraditionalBoard::hasPieceCount() const
{
    return false;
}

} // namespace Chess
