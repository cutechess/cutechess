#ifndef MANCHUBOARD_H
#define MANCHUBOARD_H
#include "easternboard.h"

namespace Chess {
/*!
 * \brief A board for Manchu chess
 *
 * Modern chess is a variant of xiangqi. It was created during the
 * Qing Dynasty by the Bannermen and was one of the most popular
 * board games among them.
 *
 * This variant is supported by Fairy-Stockfish chess engines.
 *
 * \note Rules: Black's pieces are set up and move the same as in xiangqi,
 * but horses, cannons, and one of the chariots are absent for Red. The
 * remaining chariot has the combined powers of the chariot, horse, and cannon.
 * Although Black appears to have the advantage, the lethality of the red
 * chariot can easily lead to an endgame if Black does not play cautiously.
 * The red chariot is believed to be the representation of Solon soldiers who
 * were brave and battle-hardened during the Manchu conquest of China.
 */

class LIB_EXPORT ManchuBoard : public EasternBoard
{
public:
    ManchuBoard();
    virtual Board* copy() const;
    virtual QString variant() const;
    virtual QString defaultFenString() const;

    virtual int width() const;
    virtual int height() const;
protected:
    /*! Special piece types for Manchu variants. */
    enum ManchuPieceType
    {
        //!< Banner, The Most powerfull piece in Manchu
        Banner = Pawn + 1,
    };

    virtual bool hasCastling() const;

private:

};

} // namespace Chess
#endif // MANCHUBOARD_H
