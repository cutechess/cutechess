/*
Copyright (c) 2015 basil00
Modifications Copyright (c) 2016-2020 by Jon Dart

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define TB_PAWN 1
#define TB_KNIGHT 2
#define TB_BISHOP 3
#define TB_ROOK 4
#define TB_QUEEN 5
#define TB_KING 6

#define TB_WPAWN TB_PAWN
#define TB_BPAWN (TB_PAWN | 8)

#define WHITE_KING              (TB_WPAWN + 5)
#define WHITE_QUEEN             (TB_WPAWN + 4)
#define WHITE_ROOK              (TB_WPAWN + 3)
#define WHITE_BISHOP            (TB_WPAWN + 2)
#define WHITE_KNIGHT            (TB_WPAWN + 1)
#define WHITE_PAWN              TB_WPAWN
#define BLACK_KING              (TB_BPAWN + 5)
#define BLACK_QUEEN             (TB_BPAWN + 4)
#define BLACK_ROOK              (TB_BPAWN + 3)
#define BLACK_BISHOP            (TB_BPAWN + 2)
#define BLACK_KNIGHT            (TB_BPAWN + 1)
#define BLACK_PAWN              TB_BPAWN

#define PRIME_WHITE_QUEEN       11811845319353239651ull
#define PRIME_WHITE_ROOK        10979190538029446137ull
#define PRIME_WHITE_BISHOP      12311744257139811149ull
#define PRIME_WHITE_KNIGHT      15202887380319082783ull
#define PRIME_WHITE_PAWN        17008651141875982339ull
#define PRIME_BLACK_QUEEN       15484752644942473553ull
#define PRIME_BLACK_ROOK        18264461213049635989ull
#define PRIME_BLACK_BISHOP      15394650811035483107ull
#define PRIME_BLACK_KNIGHT      13469005675588064321ull
#define PRIME_BLACK_PAWN        11695583624105689831ull

#define BOARD_RANK_EDGE         0x8181818181818181ull
#define BOARD_FILE_EDGE         0xFF000000000000FFull
#define BOARD_EDGE              (BOARD_RANK_EDGE | BOARD_FILE_EDGE)
#define BOARD_RANK_1            0x00000000000000FFull
#define BOARD_FILE_A            0x8080808080808080ull

#define KEY_KvK                 0

#define BEST_NONE               0xFFFF
#define SCORE_ILLEGAL           0x7FFF

// Note: WHITE, BLACK values are reverse of Stockfish
#ifdef __cplusplus
namespace {
enum Color { BLACK, WHITE };
enum PieceType { PAWN=1, KNIGHT, BISHOP, ROOK, QUEEN, KING };
enum Piece {
  W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
  B_PAWN = 9, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING };
#else
typedef enum Color { BLACK, WHITE } Color;
typedef enum PieceType { PAWN=1, KNIGHT, BISHOP, ROOK, QUEEN, KING } PieceType;
typedef enum Piece {
  W_PAWN = 1, W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
  B_PAWN = 9, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING
} Piece;
#endif

static inline Color ColorOfPiece(int piece) {
  return (Color)(!(piece >> 3));
}

static inline PieceType TypeOfPiece(int piece) {
  return (PieceType)(piece & 7);
}

typedef int32_t Value;

typedef struct Pos
{
    uint64_t white;
    uint64_t black;
    uint64_t kings;
    uint64_t queens;
    uint64_t rooks;
    uint64_t bishops;
    uint64_t knights;
    uint64_t pawns;
    uint8_t rule50;
    uint8_t ep;
    bool turn;
} Pos;

static inline uint64_t pieces_by_type(const Pos *pos, Color c, PieceType p) {
  uint64_t mask = (c == WHITE) ? pos->white : pos->black;
  switch(p) {
  case PAWN:
    return pos->pawns & mask;
  case KNIGHT:
    return pos->knights & mask;
  case BISHOP:
    return pos->bishops & mask;
  case ROOK:
    return pos->rooks & mask;
  case QUEEN:
    return pos->queens & mask;
  case KING:
    return pos->kings & mask;
  default:
    assert(0);
    return 0;
  }
}

static const char piece_to_char[] = " PNBRQK  pnbrqk";
  
// map upper-case characters to piece types
static PieceType char_to_piece_type(char c) {
    for (int i = PAWN; i <= KING; i++)
       if (c == piece_to_char[i]) {
         return (PieceType)i;
       }
    return (PieceType)0;
}

#define rank(s)                 ((s) >> 3)
#define file(s)                 ((s) & 0x07)
#define board(s)                ((uint64_t)1 << (s))
#define square(r, f)            (8 * (r) + (f))

#ifdef TB_KING_ATTACKS
#define king_attacks(s)         TB_KING_ATTACKS(s)
#define king_attacks_init()     /* NOP */
#else       /* TB_KING_ATTACKS */

static uint64_t king_attacks_table[64];

#define king_attacks(s)         king_attacks_table[(s)]

static void king_attacks_init(void)
{
    for (unsigned s = 0; s < 64; s++)
    {
        unsigned r = rank(s);
        unsigned f = file(s);
        uint64_t b = 0;
        if (r != 0 && f != 0)
            b |= board(square(r-1, f-1));
        if (r != 0)
            b |= board(square(r-1, f));
        if (r != 0 && f != 7)
            b |= board(square(r-1, f+1));
        if (f != 7)
            b |= board(square(r, f+1));
        if (r != 7 && f != 7)
            b |= board(square(r+1, f+1));
        if (r != 7)
            b |= board(square(r+1, f));
        if (r != 7 && f != 0)
            b |= board(square(r+1, f-1));
        if (f != 0)
            b |= board(square(r, f-1));
        king_attacks_table[s] = b;
    }
}

#endif      /* TB_KING_ATTACKS */

#ifdef TB_KNIGHT_ATTACKS
#define knight_attacks(s)       TB_KNIGHT_ATTACKS(s)
#define knight_attacks_init()   /* NOP */
#else       /* TB_KNIGHT_ATTACKS */

static uint64_t knight_attacks_table[64];

#define knight_attacks(s)       knight_attacks_table[(s)]

static void knight_attacks_init(void)
{
    for (unsigned s = 0; s < 64; s++)
    {
        int r1, r = rank(s);
        int f1, f = file(s);
        uint64_t b = 0;
        r1 = r-1; f1 = f-2;
        if (r1 >= 0 && f1 >= 0)
            b |= board(square(r1, f1));
        r1 = r-1; f1 = f+2;
        if (r1 >= 0 && f1 <= 7)
            b |= board(square(r1, f1));
        r1 = r-2; f1 = f-1;
        if (r1 >= 0 && f1 >= 0)
            b |= board(square(r1, f1));
        r1 = r-2; f1 = f+1;
        if (r1 >= 0 && f1 <= 7)
            b |= board(square(r1, f1));
        r1 = r+1; f1 = f-2;
        if (r1 <= 7 && f1 >= 0)
            b |= board(square(r1, f1));
        r1 = r+1; f1 = f+2;
        if (r1 <= 7 && f1 <= 7)
            b |= board(square(r1, f1));
        r1 = r+2; f1 = f-1;
        if (r1 <= 7 && f1 >= 0)
            b |= board(square(r1, f1));
        r1 = r+2; f1 = f+1;
        if (r1 <= 7 && f1 <= 7)
            b |= board(square(r1, f1));
        knight_attacks_table[s] = b;
    }
}

#endif      /* TB_KNIGHT_ATTACKS */

#ifdef TB_BISHOP_ATTACKS
#define bishop_attacks(s, occ)  TB_BISHOP_ATTACKS(s, occ)
#define bishop_attacks_init()   /* NOP */
#else       /* TB_BISHOP_ATTACKS */

static uint64_t diag_attacks_table[64][64];
static uint64_t anti_attacks_table[64][64];

static const unsigned square2diag_table[64] =
{
    0,  1,  2,  3,  4,  5,  6,  7,
    14, 0,  1,  2,  3,  4,  5,  6,
    13, 14, 0,  1,  2,  3,  4,  5,
    12, 13, 14, 0,  1,  2,  3,  4,
    11, 12, 13, 14, 0,  1,  2,  3,
    10, 11, 12, 13, 14, 0,  1,  2,
    9,  10, 11, 12, 13, 14, 0,  1,
    8,  9,  10, 11, 12, 13, 14, 0
};

static const unsigned square2anti_table[64] =
{
    8,  9,  10, 11, 12, 13, 14, 0,
    9,  10, 11, 12, 13, 14, 0,  1,
    10, 11, 12, 13, 14, 0,  1,  2,
    11, 12, 13, 14, 0,  1,  2,  3,
    12, 13, 14, 0,  1,  2,  3,  4,
    13, 14, 0,  1,  2,  3,  4,  5,
    14, 0,  1,  2,  3,  4,  5,  6,
    0,  1,  2,  3,  4,  5,  6,  7
};

static const uint64_t diag2board_table[15] =
{
    0x8040201008040201ull,
    0x0080402010080402ull,
    0x0000804020100804ull,
    0x0000008040201008ull,
    0x0000000080402010ull,
    0x0000000000804020ull,
    0x0000000000008040ull,
    0x0000000000000080ull,
    0x0100000000000000ull,
    0x0201000000000000ull,
    0x0402010000000000ull,
    0x0804020100000000ull,
    0x1008040201000000ull,
    0x2010080402010000ull,
    0x4020100804020100ull,
};

static const uint64_t anti2board_table[15] =
{
    0x0102040810204080ull,
    0x0204081020408000ull,
    0x0408102040800000ull,
    0x0810204080000000ull,
    0x1020408000000000ull,
    0x2040800000000000ull,
    0x4080000000000000ull,
    0x8000000000000000ull,
    0x0000000000000001ull,
    0x0000000000000102ull,
    0x0000000000010204ull,
    0x0000000001020408ull,
    0x0000000102040810ull,
    0x0000010204081020ull,
    0x0001020408102040ull,
};

static inline size_t diag2index(uint64_t b)
{
    b *= 0x0101010101010101ull;
    b >>= 56;
    b >>= 1;
    return (size_t)b;
}

static inline size_t anti2index(uint64_t b)
{
    return diag2index(b);
}

#define diag(s)                 square2diag_table[(s)]
#define anti(s)                 square2anti_table[(s)]
#define diag2board(d)           diag2board_table[(d)]
#define anti2board(a)           anti2board_table[(a)]

static uint64_t bishop_attacks(unsigned sq, uint64_t occ)
{
    occ &= ~board(sq);
    unsigned d = diag(sq), a = anti(sq);
    uint64_t d_occ = occ & (diag2board(d) & ~BOARD_EDGE);
    uint64_t a_occ = occ & (anti2board(a) & ~BOARD_EDGE);
    size_t d_idx = diag2index(d_occ);
    size_t a_idx = anti2index(a_occ);
    uint64_t d_attacks = diag_attacks_table[sq][d_idx];
    uint64_t a_attacks = anti_attacks_table[sq][a_idx];
    return d_attacks | a_attacks;
}

static void bishop_attacks_init(void)
{
    for (unsigned idx = 0; idx < 64; idx++)
    {
        unsigned idx1 = idx << 1;
        for (unsigned s = 0; s < 64; s++)
        {
            int r = rank(s);
            int f = file(s);
            uint64_t b = 0;
            for (int i = -1; f + i >= 0 && r + i >= 0; i--)
            {
                unsigned occ = (1 << (f + i));
                b |= board(square(r + i, f + i));
                if (idx1 & occ)
                    break;
            }
            for (int i = 1; f + i <= 7 && r + i <= 7; i++)
            {
                unsigned occ = (1 << (f + i));
                b |= board(square(r + i, f + i));
                if (idx1 & occ)
                    break;
            }
            diag_attacks_table[s][idx] = b;
        }
    }

    for (unsigned idx = 0; idx < 64; idx++)
    {
        unsigned idx1 = idx << 1;
        for (unsigned s = 0; s < 64; s++)
        {
            int r = rank(s);
            int f = file(s);
            uint64_t b = 0;
            for (int i = -1; f + i >= 0 && r - i <= 7; i--)
            {
                unsigned occ = (1 << (f + i));
                b |= board(square(r - i, f + i));
                if (idx1 & occ)
                    break;
            }
            for (int i = 1; f + i <= 7 && r - i >= 0; i++)
            {
                unsigned occ = (1 << (f + i));
                b |= board(square(r - i, f + i));
                if (idx1 & occ)
                    break;
            }
            anti_attacks_table[s][idx] = b;
        }
    }
}

#endif      /* TB_BISHOP_ATTACKS */

#ifdef TB_ROOK_ATTACKS
#define rook_attacks(s, occ)    TB_ROOK_ATTACKS(s, occ)
#define rook_attacks_init()     /* NOP */
#else       /* TB_ROOK_ATTACKS */

static uint64_t rank_attacks_table[64][64];
static uint64_t file_attacks_table[64][64];

static inline size_t rank2index(uint64_t b, unsigned r)
{
    b >>= (8 * r);
    b >>= 1;
    return (size_t)b;
}

static inline size_t file2index(uint64_t b, unsigned f)
{
    b >>= f;
    b *= 0x0102040810204080ull;
    b >>= 56;
    b >>= 1;
    return (size_t)b;
}

#define rank2board(r)           (0xFFull << (8 * (r)))
#define file2board(f)           (0x0101010101010101ull << (f))

static uint64_t rook_attacks(unsigned sq, uint64_t occ)
{
    occ &= ~board(sq);
    unsigned r = rank(sq), f = file(sq);
    uint64_t r_occ = occ & (rank2board(r) & ~BOARD_RANK_EDGE);
    uint64_t f_occ = occ & (file2board(f) & ~BOARD_FILE_EDGE);
    size_t r_idx = rank2index(r_occ, r);
    size_t f_idx = file2index(f_occ, f);
    uint64_t r_attacks = rank_attacks_table[sq][r_idx];
    uint64_t f_attacks = file_attacks_table[sq][f_idx];
    return r_attacks | f_attacks;
}

static void rook_attacks_init(void)
{
    for (unsigned idx = 0; idx < 64; idx++)
    {
        unsigned idx1 = idx << 1, occ;
        for (int f = 0; f <= 7; f++)
        {
            uint64_t b = 0;
            if (f > 0)
            {
                int i = f-1;
                do
                {
                    occ = (1 << i);
                    b |= board(square(0, i));
                    i--;
                }
                while (!(idx1 & occ) && i >= 0);
            }
            if (f < 7)
            {
                int i = f+1;
                do
                {
                    occ = (1 << i);
                    b |= board(square(0, i));
                    i++;
                }
                while (!(idx1 & occ) && i <= 7);
            }
            for (int r = 0; r <= 7; r++)
            {
                rank_attacks_table[square(r, f)][idx] = b;
                b <<= 8;
            }
        }
    }
    for (unsigned idx = 0; idx < 64; idx++)
    {
        unsigned idx1 = idx << 1, occ;
        for (int r = 0; r <= 7; r++)
        {
            uint64_t b = 0;
            if (r > 0)
            {
                int i = r-1;
                do
                {
                    occ = (1 << i);
                    b |= board(square(i, 0));
                    i--;
                }
                while (!(idx1 & occ) && i >= 0);
            }
            if (r < 7)
            {
                int i = r+1;
                do
                {
                    occ = (1 << i);
                    b |= board(square(i, 0));
                    i++;
                }
                while (!(idx1 & occ) && i <= 7);
            }
            for (int f = 0; f <= 7; f++)
            {
                file_attacks_table[square(r, f)][idx] = b;
                b <<= 1;
            }
        }
    }
}

#endif      /* TB_ROOK_ATTACKS */

#ifdef TB_QUEEN_ATTACKS
#define queen_attacks(s, occ)   TB_QUEEN_ATTACKS(s, occ)
#else       /* TB_QUEEN_ATTACKS */
#define queen_attacks(s, occ)   \
    (rook_attacks((s), (occ)) | bishop_attacks((s), (occ)))
#endif      /* TB_QUEEN_ATTACKS */

#ifdef TB_PAWN_ATTACKS
#define pawn_attacks(s, c)      TB_PAWN_ATTACKS(s, c)
#define pawn_attacks_init()     /* NOP */
#else       /* TB_PAWN_ATTACKS */

static uint64_t pawn_attacks_table[2][64];

#define pawn_attacks(s, c)      pawn_attacks_table[(c)][(s)]

static void pawn_attacks_init(void)
{
    for (unsigned s = 0; s < 64; s++)
    {
        int r = rank(s);
        int f = file(s);

        uint64_t b = 0;
        if (r != 7)
        {
            if (f != 0)
                b |= board(square(r+1, f-1));
            if (f != 7)
                b |= board(square(r+1, f+1));
        }
        pawn_attacks_table[1][s] = b;

        b = 0;
        if (r != 0)
        {
            if (f != 0)
                b |= board(square(r-1, f-1));
            if (f != 7)
                b |= board(square(r-1, f+1));
        }
        pawn_attacks_table[0][s] = b;
    }
}

#endif      /* TB_PAWN_ATTACKS */

/*
 * Given a position, produce a 64-bit material signature key.
 */
static uint64_t calc_key(const Pos *pos, bool mirror)
{
    uint64_t white = pos->white, black = pos->black;
    if (mirror)
    {
        uint64_t tmp = white;
        white = black;
        black = tmp;
    }
    return popcount(white & pos->queens)  * PRIME_WHITE_QUEEN +
           popcount(white & pos->rooks)   * PRIME_WHITE_ROOK +
           popcount(white & pos->bishops) * PRIME_WHITE_BISHOP +
           popcount(white & pos->knights) * PRIME_WHITE_KNIGHT +
           popcount(white & pos->pawns)   * PRIME_WHITE_PAWN +
           popcount(black & pos->queens)  * PRIME_BLACK_QUEEN +
           popcount(black & pos->rooks)   * PRIME_BLACK_ROOK +
           popcount(black & pos->bishops) * PRIME_BLACK_BISHOP +
           popcount(black & pos->knights) * PRIME_BLACK_KNIGHT +
           popcount(black & pos->pawns)   * PRIME_BLACK_PAWN;
}

// Produce a 64-bit material key corresponding to the material combination
// defined by pcs[16], where pcs[1], ..., pcs[6] are the number of white
// pawns, ..., kings and pcs[9], ..., pcs[14] are the number of black
// pawns, ..., kings.
static uint64_t calc_key_from_pcs(int *pcs, int mirror)
{
    mirror = (mirror? 8: 0);
    return pcs[WHITE_QUEEN ^ mirror] * PRIME_WHITE_QUEEN +
           pcs[WHITE_ROOK ^ mirror] * PRIME_WHITE_ROOK +
           pcs[WHITE_BISHOP ^ mirror] * PRIME_WHITE_BISHOP +
           pcs[WHITE_KNIGHT ^ mirror] * PRIME_WHITE_KNIGHT +
           pcs[WHITE_PAWN ^ mirror] * PRIME_WHITE_PAWN +
           pcs[BLACK_QUEEN ^ mirror] * PRIME_BLACK_QUEEN +
           pcs[BLACK_ROOK ^ mirror] * PRIME_BLACK_ROOK +
           pcs[BLACK_BISHOP ^ mirror] * PRIME_BLACK_BISHOP +
           pcs[BLACK_KNIGHT ^ mirror] * PRIME_BLACK_KNIGHT +
           pcs[BLACK_PAWN ^ mirror] * PRIME_BLACK_PAWN;
}

// Produce a 64-bit material key corresponding to the material combination
// piece[0], ..., piece[num - 1], where each value corresponds to a piece
// (1-6 for white pawn-king, 9-14 for black pawn-king).
static uint64_t calc_key_from_pieces(uint8_t *piece, int num)
{
    uint64_t key = 0;
    static const uint64_t keys[16] = {0,PRIME_WHITE_PAWN,PRIME_WHITE_KNIGHT,
                                      PRIME_WHITE_BISHOP,PRIME_WHITE_ROOK,
                                      PRIME_WHITE_QUEEN,0,0,PRIME_BLACK_PAWN,
                                      PRIME_BLACK_KNIGHT,PRIME_BLACK_BISHOP,
                                      PRIME_BLACK_ROOK,PRIME_BLACK_QUEEN,0};
    for (int i = 0; i < num; i++) {
      assert(piece[i]<16);
      key += keys[piece[i]];
    }
    return key;
}

#define make_move(promote, from, to)                                    \
    ((((promote) & 0x7) << 12) | (((from) & 0x3F) << 6) | ((to) & 0x3F))
#define move_from(move)                                                 \
    (((move) >> 6) & 0x3F)
#define move_to(move)                                                   \
    ((move) & 0x3F)
#define move_promotes(move)                                             \
    (((move) >> 12) & 0x7)

static inline int type_of_piece_moved(Pos *pos, TbMove move) {
  for (int i = PAWN; i <= KING; i++) {
    if ((pieces_by_type(pos,(Color)(pos->turn == WHITE),(PieceType)i) & board(move_from(move))) != 0) {
      return i;
    }
  }
  assert(0);
  return 0;
}

#define MAX_MOVES               TB_MAX_MOVES
#define MOVE_STALEMATE          0xFFFF
#define MOVE_CHECKMATE          0xFFFE

static TbMove *add_move(TbMove *moves, bool promotes, unsigned from,
    unsigned to)
{
    if (!promotes)
        *moves++ = make_move(TB_PROMOTES_NONE, from, to);
    else
    {
        *moves++ = make_move(TB_PROMOTES_QUEEN, from, to);
        *moves++ = make_move(TB_PROMOTES_KNIGHT, from, to);
        *moves++ = make_move(TB_PROMOTES_ROOK, from, to);
        *moves++ = make_move(TB_PROMOTES_BISHOP, from, to);
    }
    return moves;
}

/*
 * Generate all captures, including all underpomotions
 */
static TbMove *gen_captures(const Pos *pos, TbMove *moves)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->white: pos->black),
             them = (pos->turn? pos->black: pos->white);
    uint64_t b, att;
    {
        unsigned from = lsb(pos->kings & us);
        assert(from < 64);
        for (att = king_attacks(from) & them; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->queens; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = queen_attacks(from, occ) & them; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->rooks; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = rook_attacks(from, occ) & them; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->bishops; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = bishop_attacks(from, occ) & them; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->knights; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = knight_attacks(from) & them; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->pawns; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        att = pawn_attacks(from, pos->turn);
        if (pos->ep != 0 && ((att & board(pos->ep)) != 0))
        {
            unsigned to = pos->ep;
            moves = add_move(moves, false, from, to);
        }
        for (att = att & them; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, (rank(to) == 7 || rank(to) == 0), from,
                to);
        }
    }
    return moves;
}

/*
 * Generate all moves.
 */
static TbMove *gen_moves(const Pos *pos, TbMove *moves)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->white: pos->black),
             them = (pos->turn? pos->black: pos->white);
    uint64_t b, att;
    
    {
        unsigned from = lsb(pos->kings & us);
        for (att = king_attacks(from) & ~us; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->queens; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = queen_attacks(from, occ) & ~us; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->rooks; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = rook_attacks(from, occ) & ~us; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->bishops; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = bishop_attacks(from, occ) & ~us; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->knights; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        for (att = knight_attacks(from) & ~us; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, false, from, to);
        }
    }
    for (b = us & pos->pawns; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        unsigned next = from + (pos->turn? 8: -8);
        att = pawn_attacks(from, pos->turn);
        if (pos->ep != 0 && ((att & board(pos->ep)) != 0))
        {
            unsigned to = pos->ep;
            moves = add_move(moves, false, from, to);
        }
        att &= them;
        if ((board(next) & occ) == 0)
        {
            att |= board(next);
            unsigned next2 = from + (pos->turn? 16: -16);
            if ((pos->turn? rank(from) == 1: rank(from) == 6) &&
                    ((board(next2) & occ) == 0))
                att |= board(next2);
        }
        for (; att; att = poplsb(att))
        {
            unsigned to = lsb(att);
            moves = add_move(moves, (rank(to) == 7 || rank(to) == 0), from,
                to);
        }
    }
    return moves;
}

/*
 * Test if the given move is an en passant capture.
 */
static bool is_en_passant(const Pos *pos, TbMove move)
{
    uint16_t from = move_from(move);
    uint16_t to   = move_to(move);
    uint64_t us = (pos->turn? pos->white: pos->black);
    if (pos->ep == 0)
        return false;
    if (to != pos->ep)
        return false;
    if ((board(from) & us & pos->pawns) == 0)
        return false;
    return true;
}


/*
 * Test if the given move is a capture.
 */
static bool is_capture(const Pos *pos, TbMove move)
{
   uint16_t to   = move_to(move);
   uint64_t them = (pos->turn? pos->black: pos->white);
   return (them & board(to)) != 0 || is_en_passant(pos,move);
}


/*
 * Test if the given position is legal.
 * (Pawns on backrank? Can the king be captured?)
 */
static bool is_legal(const Pos *pos)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->black: pos->white),
             them = (pos->turn? pos->white: pos->black);
    uint64_t king = pos->kings & us;
    if (!king)
        return false;
    unsigned sq = lsb(king);
    if (king_attacks(sq) & (pos->kings & them))
        return false;
    uint64_t ratt = rook_attacks(sq, occ);
    uint64_t batt = bishop_attacks(sq, occ);
    if (ratt & (pos->rooks & them))
        return false;
    if (batt & (pos->bishops & them))
        return false;
    if ((ratt | batt) & (pos->queens & them))
        return false;
    if (knight_attacks(sq) & (pos->knights & them))
        return false;
    if (pawn_attacks(sq, !pos->turn) & (pos->pawns & them))
        return false;
    return true;
}

/*
 * Test if the king is in check.
 */
static bool is_check(const Pos *pos)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->white: pos->black),
             them = (pos->turn? pos->black: pos->white);
    uint64_t king = pos->kings & us;
    assert(king != 0);
    unsigned sq = lsb(king);
    uint64_t ratt = rook_attacks(sq, occ);
    uint64_t batt = bishop_attacks(sq, occ);
    if (ratt & (pos->rooks & them))
        return true;
    if (batt & (pos->bishops & them))
        return true;
    if ((ratt | batt) & (pos->queens & them))
        return true;
    if (knight_attacks(sq) & (pos->knights & them))
        return true;
    if (pawn_attacks(sq, pos->turn) & (pos->pawns & them))
        return true;
    return false;
}

/*
 * Test if the position is valid.
 */
static bool is_valid(const Pos *pos)
{
    if (popcount(pos->kings) != 2)
        return false;
    if (popcount(pos->kings & pos->white) != 1)
        return false;
    if (popcount(pos->kings & pos->black) != 1)
        return false;
    if ((pos->white & pos->black) != 0)
        return false;
    if ((pos->kings & pos->queens) != 0)
        return false;
    if ((pos->kings & pos->rooks) != 0)
        return false;
    if ((pos->kings & pos->bishops) != 0)
        return false;
    if ((pos->kings & pos->knights) != 0)
        return false;
    if ((pos->kings & pos->pawns) != 0)
        return false;
    if ((pos->queens & pos->rooks) != 0)
        return false;
    if ((pos->queens & pos->bishops) != 0)
        return false;
    if ((pos->queens & pos->knights) != 0)
        return false;
    if ((pos->queens & pos->pawns) != 0)
        return false;
    if ((pos->rooks & pos->bishops) != 0)
        return false;
    if ((pos->rooks & pos->knights) != 0)
        return false;
    if ((pos->rooks & pos->pawns) != 0)
        return false;
    if ((pos->bishops & pos->knights) != 0)
        return false;
    if ((pos->bishops & pos->pawns) != 0)
        return false;
    if ((pos->knights & pos->pawns) != 0)
        return false;
    if (pos->pawns & BOARD_FILE_EDGE)
        return false;
    if ((pos->white | pos->black) !=
        (pos->kings | pos->queens | pos->rooks | pos->bishops | pos->knights |
         pos->pawns))
        return false;
    return is_legal(pos);
}

#define do_bb_move(b, from, to)                                         \
    (((b) & (~board(to)) & (~board(from))) |                            \
        ((((b) >> (from)) & 0x1) << (to)))

static bool do_move(Pos *pos, const Pos *pos0, TbMove move)
{
    unsigned from = move_from(move); 
    unsigned to = move_to(move);  
    unsigned promotes = move_promotes(move);  
    pos->turn = !pos0->turn;
    pos->white = do_bb_move(pos0->white, from, to);
    pos->black = do_bb_move(pos0->black, from, to);
    pos->kings = do_bb_move(pos0->kings, from, to);
    pos->queens = do_bb_move(pos0->queens, from, to); 
    pos->rooks = do_bb_move(pos0->rooks, from, to);
    pos->bishops = do_bb_move(pos0->bishops, from, to);  
    pos->knights = do_bb_move(pos0->knights, from, to);  
    pos->pawns = do_bb_move(pos0->pawns, from, to);
    pos->ep = 0;
    if (promotes != TB_PROMOTES_NONE) 
    {  
        pos->pawns &= ~board(to);       // Promotion
        switch (promotes)
        { 
            case TB_PROMOTES_QUEEN:
                pos->queens |= board(to); break;
            case TB_PROMOTES_ROOK: 
                pos->rooks |= board(to); break; 
            case TB_PROMOTES_BISHOP:  
                pos->bishops |= board(to); break;  
            case TB_PROMOTES_KNIGHT:  
                pos->knights |= board(to); break;  
        }
        pos->rule50 = 0;
    }
    else if ((board(from) & pos0->pawns) != 0)
    {
        pos->rule50 = 0;                // Pawn move
        if (rank(from) == 1 && rank(to) == 3 &&
            (pawn_attacks(from+8, true) & pos0->pawns & pos0->black) != 0)
            pos->ep = from+8;
        else if (rank(from) == 6 && rank(to) == 4 &&
            (pawn_attacks(from-8, false) & pos0->pawns & pos0->white) != 0)
            pos->ep = from-8;
        else if (to == pos0->ep)
        {
            unsigned ep_to = (pos0->turn? to-8: to+8);
            uint64_t ep_mask = ~board(ep_to);
            pos->white &= ep_mask;
            pos->black &= ep_mask;
            pos->pawns &= ep_mask;
        }
    }
    else if ((board(to) & (pos0->white | pos0->black)) != 0)
        pos->rule50 = 0;                // Capture
    else
        pos->rule50 = pos0->rule50 + 1; // Normal move
    if (!is_legal(pos))
        return false;
    return true;
}

static bool legal_move(const Pos *pos, TbMove move) {
   struct Pos pos1;
   return do_move(&pos1, pos, move);
}

/*
 * Test if the king is in checkmate.
 */
static bool is_mate(const Pos *pos)
{
    if (!is_check(pos))
        return false;
    uint16_t moves0[MAX_MOVES];
    uint16_t *moves = moves0;
    uint16_t *end = gen_moves(pos, moves);
    for (; moves < end; moves++)
    {
        Pos pos1;
        if (do_move(&pos1, pos, *moves))
            return false;
    }
    return true;
}

/*
 * Generate all legal moves.
 */
static TbMove *gen_legal(const Pos *pos, TbMove *moves)
{
  TbMove pl_moves[TB_MAX_MOVES];
  TbMove *end = gen_moves(pos, pl_moves);
  TbMove *results = moves;
  for (TbMove *m = pl_moves; m < end; m++) {
    if (legal_move(pos,*m)) {
      *results++ = *m;
    }
  }
  return results;
}

#ifdef __cplusplus
};
#endif

