#ifndef JANGGIBOARD_H
#define JANGGIBOARD_H
#include "koreanboard.h"

namespace Chess {

/*!
 * \brief A board for Janggi(Korean chess)
 *
 * There are four supported rule sets, reflecting all combinations of
 * bikjang enabled/disabled and material counting enabled/disabled:
 *
 * 1.janggi: Tournament rules with bikjang and material enabled.
 * 2.janggitraditional: Traditional rules with bikjang ruled as a draw
 *   and without material counting.
 * 3.janggimodern: Modern rules equivalent to Kakao Janggi, with
 *   bikjang disabled and material counting enabled.
 * 4.janggicasual: No bikjang or material counting.
 *
 * This is the Korean chess variant, and one that is
 * supported by Fairy-stockfish engines.
 *
 * \note Rules: https://www.pychess.org/variant/janggi
 */

class LIB_EXPORT JanggiBoard : public KoreanBoard
{
    public:
        /*! Creates a new JanggiBoard object. */
        JanggiBoard();

        // Inherited from KoreanBoard
        virtual Board* copy() const;
        virtual QString variant() const;
        virtual QString defaultFenString() const;
        virtual bool hasBikjang() const;
        virtual bool hasPieceCount() const;
    protected:

    private:

};

class LIB_EXPORT JanggiCasualBoard : public KoreanBoard
{
    public:
        /*! Creates a new JanggiCasualBoard object. */
        JanggiCasualBoard();

        // Inherited from KoreanBoard
        virtual Board* copy() const;
        virtual QString variant() const;
        virtual QString defaultFenString() const;
        virtual bool hasBikjang() const;
        virtual bool hasPieceCount() const;
    protected:

    private:

};

class LIB_EXPORT JanggiModernBoard : public KoreanBoard
{
    public:
        /*! Creates a new JanggiModernBoard object. */
        JanggiModernBoard();

        // Inherited from KoreanBoard
        virtual Board* copy() const;
        virtual QString variant() const;
        virtual QString defaultFenString() const;
        virtual bool hasBikjang() const;
        virtual bool hasPieceCount() const;
    protected:

    private:

};

class LIB_EXPORT JanggiTraditionalBoard : public KoreanBoard
{
    public:
        /*! Creates a new JanggiTraditionalBoard object. */
        JanggiTraditionalBoard();

        // Inherited from KoreanBoard
        virtual Board* copy() const;
        virtual QString variant() const;
        virtual QString defaultFenString() const;
        virtual bool hasBikjang() const;
        virtual bool hasPieceCount() const;
    protected:

    private:

};

} // namespace Chess
#endif // JIANGGIBOARD_H
