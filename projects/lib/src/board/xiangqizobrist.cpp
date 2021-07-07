#include "xiangqizobrist.h"
#include <QMutexLocker>
#include "piece.h"

namespace Chess {

XiangqiZobrist::XiangqiZobrist(const quint64 *keys)
  : EasternZobrist(keys),
    m_pieceIndex(0)
{

}

void XiangqiZobrist::initialize(int squareCount, int pieceTypeCount)
{
    QMutexLocker locker(&m_mutex);

    if (isInitialized())
        return;

    EasternZobrist::initialize(squareCount, pieceTypeCount);
}

quint64 XiangqiZobrist::side() const
{
    const quint64 zobristSide = 0xA0CE2AF90C452F58;
    return zobristSide;
}

quint64 XiangqiZobrist::piece(const Piece &piece, int square) const
{
    Q_ASSERT(piece.isValid());
    Q_ASSERT(piece.type() >= 0 && piece.type() < pieceTypeCount());
    Q_ASSERT(square >= 0 && square < squareCount());
    int offset = 3;
    int arrWidth = 9 + 2;
    int file = square % arrWidth - 1;
    int rank = (10 - 1) - (square / arrWidth - 2);
    int squareZobrist = (rank + offset) * 16 + (file + offset);

    int i = m_pieceIndex + squareCount() * (pieceTypeCount()-1)* (1 - piece.side()) +
        (piece.type() - 1) * squareCount() + squareZobrist;

    return keys()[i];
}

}
