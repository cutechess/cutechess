#ifndef MINIXIANGQIBOARD_H
#define MINIXIANGQIBOARD_H
#include "easternboard.h"

namespace Chess {
/*!
 * \brief A board for Minixiangqi chess
 * Xiangqi on a 7x7 board, without Elephants and Mandarins. The object is to
 * checkmate the opponent's General by attacking it so that it has no safe
 * positions to move to. It's also a win to stalemate your opponent so that
 * he can't move - this usually only happens when a player is reduced to a
 * lone king. A player may not force a repetition of moves. Generals cannot
 * face each other on a file with no intervening pieces. The 3x3 boxes at the
 * top and bottom of the board are the Generals' imperial palaces or
 * fortresses. The Generals may not leave their fortresses. Chariots are the
 * most valuable pieces. The Horse is less valuable than the Cannon in the
 * opening, but becomes stronger as the game progresses. The Cannons are
 * effective positioned behind friendly pawns. MiniXiangqi was introduced by
 * S. Kusumoto, in 1973. There exists a MiniXiangqi association in Japan.
 * Four game examples between human players are provided.
 *
 * \note Rules: http://mlwi.magix.net/bg/minixiangqi.htm
 * This board implement the basic rules for minixiangqi, and no promotion
 * rules was supported.
 *
 * \sa MiniXiangqiBoard
 */
class LIB_EXPORT MiniXiangqiBoard : public EasternBoard
{
public:
    MiniXiangqiBoard();
    virtual Board* copy() const;
    virtual QString variant() const;
    virtual QString defaultFenString() const;

    virtual int width() const;
    virtual int height() const;

protected:
    /*! Special piece types for minixiangqi variants. */
    enum MiniXiangqiPieceType
    {
        General = Pawn + 1,    //!< King, also named Gold
        Soldier   //!< Pawn, also named Soldier
    };
    /*! Movement mask for General moves. */
    static const unsigned GeneralMovement = 16;
    /*! Movement mask for Soldier moves. */
    static const unsigned SoldierMovement = 64;

    // Inherited from EasternBoard
    virtual bool hasCastling() const;
    virtual bool pawnHasDoubleStep() const;
    virtual void vInitialize();
    virtual bool inCheck(Side side, int square = 0) const;
    virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
                       int pieceType,
                       int square) const;

private:
    virtual void generateSoldierMoves(int sourceSquare,
                   QVarLengthArray<Move>& moves) const;
    virtual void generateGeneralMoves(int sourceSquare,
                   QVarLengthArray<Move>& moves) const;
    bool inPalace(int square) const;

    int m_sign;
    bool m_pawnAmbiguous;
    bool m_multiDigitNotation;
    const EasternZobrist* m_zobrist;
    QVarLengthArray<int> m_soldierPreOffsets;
    QVarLengthArray<int> m_generalMeetOffsets;
    QVarLengthArray<int> m_crossOffsets;

};
} // namespace Chess
#endif // MINIXIANGQI_H
