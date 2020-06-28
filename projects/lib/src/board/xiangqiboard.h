#ifndef XIANGQIBOARD_H
#define XIANGQIBOARD_H
#include "easternboard.h"

namespace Chess {

/*!
 * \brief A board for Xiangqi(Chinese chess)
 *
 * This is the popular chess variant in China, and one that is
 * supported by Fairy-Stockfish, sjaakii, eleeye chess engines.
 *
 * XiangqiBoard uses Binghe Openbook zobrist position keys,
 * so Binghe opening books can be used easily.
 *
 * \note Rules: https://en.wikipedia.org/w/index.php?title=Xiangqi&oldid=930422187
 * \sa BingheBook
 */
class LIB_EXPORT XiangqiBoard : public EasternBoard
{
    public:
        /*! Creates a new StandardBoard object. */
        XiangqiBoard();

        // Inherited from EasternBoard
        virtual Board* copy() const;
        virtual QString variant() const;
        virtual QString defaultFenString() const;
    protected:

    private:

};

} // namespace Chess
#endif // XIANGQIBOARD_H
