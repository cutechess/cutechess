#ifndef KOREANBOARD_H
#define KOREANBOARD_H

#include "board.h"

namespace Chess {

class EasternZobrist;

/*!
 * \brief A virtual board for Jianggi chess variants
 *
 * KoreanBoard serves as the overclass for all Jianggi variants.
 * Jianggi has the similiar pieces as xiangqi,but some pieces have the
 * different rules.
 *
 * KoreanBoard implements the rules of Janggi variants.
 */
class LIB_EXPORT KoreanBoard : public Board
{
    public:
        /*! Basic piece types for Korean variants. */
        enum KoreanPieceType
        {
            King = 1,   //!< King, also named Gold
            Advisor,    //!< Advisor, also named Guard
            Bishop,		//!< Bishop, also named Elephant
            Knight,		//!< Knight, also named Horse
            Rook,		//!< Rook,
            Cannon,     //!< Cannon
            Pawn,       //!< Pawn, also named Soldier
        };

        /*! Creates a new KoreanBoard object. */
        KoreanBoard(EasternZobrist* zobrist);

        // Inherited from Board
        virtual int width() const;
        virtual int height() const;
        virtual Result result();
        virtual int reversibleMoveCount() const;

    protected:
        /*! The king's castling side. */
        enum CastlingSide
        {
            QueenSide,	//!< Queen side (O-O-O)
            KingSide,	//!< King side (O-O)
            NoCastlingSide	//!< No castling side
        };


        /*! Movement mask for Advisor(Guard) moves. */
        static const unsigned AdvisorMovement = 2;
        /*! Movement mask for Bishop(Elephant) moves. */
        static const unsigned BishopMovement = 4;
        /*! Movement mask for Cannon moves. */
        static const unsigned CannonMovement = 8;
        /*! Movement mask for King moves. */
        static const unsigned KingMovement = 16;
        /*! Movement mask for Knight(Horse) moves. */
        static const unsigned KnightMovement = 32;
        /*! Movement mask for Pawn(Soldier) moves. */
        static const unsigned PawnMovement = 64;
        /*! Movement mask for Rook moves. */
        static const unsigned RookMovement = 128;

        /*! Types of Pawn moves. */
        enum StepType
        {
             NoStep = 0,      //!< Cannot move here
             FreeStep =  1,   //!< May move if target is empty
             CaptureStep = 2  //!< Capture opposing piece only
        };
        /*! Stores a move \a type and a move direction \a file for a Pawn move */
        struct PawnStep { StepType type; int file; };
        /*!
         * Movement mask for Pawn moves.
         * Lists pawn move types and relative files
         * \sa BerolinaBoard
         *
         * Default: A Pawn can step straight ahead onto a free square or
         * capture diagonally forward. So initialise this
         * as { {CaptureStep, -1}, {FreeStep, 0}, {CaptureStep, 1} }
         */
        QVarLengthArray<PawnStep, 8> m_pawnSteps;
        /*!
         * Helper function for Pawn moves. Returns the count of moves
         * of the given \a type that are specified in pawnSteps.
         */
        int pawnAmbiguity(StepType type = FreeStep) const;
        /*!
         * Returns true if both counts of kings given by
         * \a whiteKings and \a blackKings are correct.
         * WesternBoard expects exactly one king per side.
         * \sa AntiBoard
         * \sa HordeBoard
         */
        virtual bool kingsCountAssertion(int whiteKings,
                         int blackKings) const;
        /*!
         * Returns true if the king can capture opposing pieces.
         * The default value is true.
         * \sa AtomicBoard
         */
        virtual bool kingCanCapture() const;
        /*!
        * Returns true if castling is allowed.
        * The default value is true.
        * \sa ShatranjBoard
        */
        virtual bool hasCastling() const;

        virtual bool hasBikjang() const;
        virtual bool hasPieceCount() const;

        /*!
         * Returns true if a rule provides \a side to insert a reserve
         * piece at a vacated source \a square immediately after a move.
         * The default value is false.
         *
         * \sa SeirawanBoard
         */
        virtual bool variantHasChanneling(Side side, int square) const;
        /*!
         * Adds pawn promotions to a move list.
         *
         * This function is called when a pawn can promote by
         * moving from \a sourceSquare to \a targetSquare.
         * This function generates all the possible promotions
         * and adds them to \a moves.
         */
        virtual void addPromotions(int sourceSquare,
                       int targetSquare,
                       QVarLengthArray<Move>& moves) const;
        /*! Returns the king square of \a side. */
        int kingSquare(Side side) const;
        /*!
         * Parse castling rights given by character \a c of the FEN
         * token. Returns true if successful.
         */
        virtual bool parseCastlingRights(QChar c);
        /*!
         * Returns true if \a side has a right to castle on \a castlingSide;
         * otherwise returns false.
         *
         * \note Even if this function returns true, castling may not be
         * a legal move in the current position.
         */
        bool hasCastlingRight(Side side, CastlingSide castlingSide) const;
        /*!
         * Removes castling rights at \a square.
         *
         * If one of the players has a rook at \a square, the rook can't
         * be used for castling. This function should be called when a
         * capture happens at \a square.
         */
        void removeCastlingRights(int square);
        /*!
         * Removes all castling rights of \a side.
         */
        void removeCastlingRights(Side side);
        /*!
         * Defines the file a king may castle to on \a castlingSide.
         * Defaults: 2 (c-file) and width() - 2 (normally g-file)
         */
        virtual int castlingFile(CastlingSide castlingSide) const;
        /*!
         * Returns true if \a side is under attack at \a square.
         * If \a square is 0, then the king square is used.
         */
        virtual bool inCheck(Side side, int square = 0) const;

        /*!
         * Returns FEN extensions. The default is an empty string.
         *
         * This function is called by fenString() via vFenString().
         * Returns additional parts of the current position's (extended)
         * FEN string which succeed the en passant field.
         */
        virtual QString vFenIncludeString(FenNotation notation) const;

        // Inherited from Board
        virtual void vInitialize();
        virtual QString vFenString(FenNotation notation) const;
        virtual bool vSetFenString(const QStringList& fen);
        virtual QString lanMoveString(const Move& move);
        virtual QString sanMoveString(const Move& move);
        virtual Move moveFromLanString(const QString& str);
        virtual Move moveFromSanString(const QString& str);

        virtual void vMakeMove(const Move& move,
                       BoardTransition* transition);
        virtual void vUndoMove(const Move& move);
        virtual void generateMovesForPiece(QVarLengthArray<Move>& moves,
                           int pieceType,
                           int square) const;
        virtual bool vIsLegalMove(const Move& move);
        virtual bool isLegalPosition();
        virtual int captureType(const Move& move) const;

    private:
        struct CastlingRights
        {
            // Usage: 'rookSquare[Side][CastlingSide]'
            // A value of zero (square 0) means no castling rights
            int rookSquare[2][2];
        };

        // Data for reversing/unmaking a move
        struct MoveData
        {
            Piece capture;
            CastlingRights castlingRights;
            CastlingSide castlingSide;
            bool bikjangState;
            int passingCounter;
            float currentScore[2];
            int reversibleMoveCount;
        };

        void generateAdvisorMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        void generateBishopMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        void generateCannonMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        void generateKingMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        void generateKnightMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        void generatePawnMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        void generateRookMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        void generateCastlingMoves(QVarLengthArray<Move>& moves) const;
        void generatePassMoves(int sourceSquare,
                       QVarLengthArray<Move>& moves) const;
        bool canCastle(CastlingSide castlingSide) const;
        QString castlingRightsString(FenNotation notation) const;
        CastlingSide castlingSide(const Move& move) const;
        void setCastlingSquare(Side side,
                       CastlingSide cside,
                       int square);
        /*! Helper for Pawn moves. Returns square offset for the
         *  given \a step with orientation \a sign. */
        inline int pawnPushOffset(const PawnStep& ps,
                      int sign) const;

        bool inFort(int square) const;
        void initCurrentScore();
        bool kingMeet(Side side, int square) const;
        Result resultFromCounting() const;
        Result resultFromBikjangState();

        int m_arwidth;
        int m_sign;
        int m_kingSquare[2];
        int m_plyOffset;
        int m_reversibleMoveCount;
        bool m_kingCanCapture;
        bool m_hasCastling;
        bool m_pawnAmbiguous;
        bool m_multiDigitNotation;
        bool m_bikjang;
        bool m_bikjangState; //indicate if the board is in bikjang State;
        bool m_pieceCount;
        bool m_enableBikjang;
        int m_passingCounter;
        float m_currentScore[2];
        QVector<MoveData> m_history;
        CastlingRights m_castlingRights;
        int m_castleTarget[2][2];

        const EasternZobrist* m_zobrist;

        QVarLengthArray<int> m_pawnPreOffsets;
        QVarLengthArray<int> m_orthogonalOffsets;
        QVarLengthArray<int> m_diagonalOffsets;
        QVarLengthArray<int> m_knightObstacleOffsets;
        QVarLengthArray<int> m_knightPreOffsets;
        QVarLengthArray<int> m_bishopOrthObstacleOffsets;
        QVarLengthArray<int> m_bishopDiagObstacleOffsets;
        QVarLengthArray<int> m_bishopPreOffsets;
        QVarLengthArray<int> m_kingMeetOffsets;
};

} // namespace Chess
#endif // EASTERNBOARD_H
