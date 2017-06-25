/*
 * tbprobe.c
 * Copyright (c) 2013-2015 Ronald de Man
 * This file may be redistributed and/or modified without restrictions.
 *
 * (C) 2015 basil, all rights reserved,
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "tbprobe.h"

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

#ifndef TB_NO_HW_POP_COUNT
#ifdef TB_CUSTOM_POP_COUNT
#define popcount(x) TB_CUSTOM_POP_COUNT(x)
#else
#include <popcntintrin.h>
#define popcount(x)             _mm_popcnt_u64((x))
#endif
#else
static inline unsigned popcount(uint64_t x)
{
    x = x - ((x >> 1) & 0x5555555555555555ull);
    x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
    x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full;
    return (x * 0x0101010101010101ull) >> 56;
}
#endif

#define poplsb(x)               ((x) & ((x) - 1))

#define make_move(promote, from, to)                                    \
    ((((promote) & 0x7) << 12) | (((from) & 0x3F) << 6) | ((to) & 0x3F))
#define move_from(move)                                                 \
    (((move) >> 6) & 0x3F)
#define move_to(move)                                                   \
    ((move) & 0x3F)
#define move_promotes(move)                                             \
    (((move) >> 12) & 0x7)

#define MAX_MOVES               TB_MAX_MOVES
#define MOVE_STALEMATE          0xFFFF
#define MOVE_CHECKMATE          0xFFFE

struct pos
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
};

static bool do_move(struct pos *pos, const struct pos *pos0, uint16_t move);
static int probe_dtz(const struct pos *pos, int *success);

unsigned TB_LARGEST = 0;
#include "tbcore.c"

#define rank(s)                 ((s) >> 3)
#define file(s)                 ((s) & 0x07)
#define board(s)                ((uint64_t)1 << (s))
#ifdef TB_CUSTOM_LSB
#define lsb(b) TB_CUSTOM_LSB(b)
#elif !defined(_WIN64) && !defined(__x86_64__)
static const int lsb_64_table[64] =
{
    63, 30,  3, 32, 59, 14, 11, 33,
    60, 24, 50,  9, 55, 19, 21, 34,
    61, 29,  2, 53, 51, 23, 41, 18,
    56, 28,  1, 43, 46, 27,  0, 35,
    62, 31, 58,  4,  5, 49, 54,  6,
    15, 52, 12, 40,  7, 42, 45, 16,
    25, 57, 48, 13, 10, 39,  8, 44,
    20, 47, 38, 22, 17, 37, 36, 26
};

/**
 * @author Matt Taylor (2003)
 * @param b bitboard to scan
 * @precondition b != 0
 * @return index (0..63) of least significant one bit
 */
static inline unsigned lsb(uint64_t b)
{
    unsigned int folded;
    b ^= b - 1;
    folded = (int) b ^ (b >> 32);
    return lsb_64_table[folded * 0x78291ACF >> 26];
}
#elif defined(_MSC_VER)
static inline unsigned lsb(uint64_t b)
{
    unsigned long ret = 0;
    _BitScanForward64(&ret, b);
    return ret;
}
#else
static inline unsigned lsb(uint64_t b)
{
    size_t idx;
    __asm__("bsfq %1, %0": "=r"(idx): "rm"(b));
    return idx;
}
#endif
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
    size_t a_idx = diag2index(a_occ);
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

static void prt_str(const struct pos *pos, char *str, bool mirror)
{
    uint64_t white = pos->white, black = pos->black;
    int i;
    if (mirror)
    {
        uint64_t tmp = white;
        white = black;
        black = tmp;
    }
    *str++ = 'K';
    for (i = popcount(white & pos->queens); i > 0; i--)
        *str++ = 'Q';
    for (i = popcount(white & pos->rooks); i > 0; i--)
        *str++ = 'R';
    for (i = popcount(white & pos->bishops); i > 0; i--)
        *str++ = 'B';
    for (i = popcount(white & pos->knights); i > 0; i--)
        *str++ = 'N';
    for (i = popcount(white & pos->pawns); i > 0; i--)
        *str++ = 'P';
    *str++ = 'v';
    *str++ = 'K';
    for (i = popcount(black & pos->queens); i > 0; i--)
        *str++ = 'Q';
    for (i = popcount(black & pos->rooks); i > 0; i--)
        *str++ = 'R';
    for (i = popcount(black & pos->bishops); i > 0; i--)
        *str++ = 'B';
    for (i = popcount(black & pos->knights); i > 0; i--)
        *str++ = 'N';
    for (i = popcount(black & pos->pawns); i > 0; i--)
        *str++ = 'P';
    *str++ = '\0';
}

/*
 * Given a position, produce a 64-bit material signature key.
 */
static uint64_t calc_key(const struct pos *pos, bool mirror)
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

static uint64_t get_pieces(const struct pos *pos, uint8_t code)
{
    switch (code)
    {
        case WHITE_KING:
            return pos->kings & pos->white;
        case WHITE_QUEEN:
            return pos->queens & pos->white;
        case WHITE_ROOK:
            return pos->rooks & pos->white;
        case WHITE_BISHOP:
            return pos->bishops & pos->white;
        case WHITE_KNIGHT:
            return pos->knights & pos->white;
        case WHITE_PAWN:
            return pos->pawns & pos->white;
        case BLACK_KING:
            return pos->kings & pos->black;
        case BLACK_QUEEN:
            return pos->queens & pos->black;
        case BLACK_ROOK:
            return pos->rooks & pos->black;
        case BLACK_BISHOP:
            return pos->bishops & pos->black;
        case BLACK_KNIGHT:
            return pos->knights & pos->black;
        case BLACK_PAWN:
            return pos->pawns & pos->black;
        default:
            return 0;   // Dummy.
    }
}

static int probe_wdl_table(const struct pos *pos, int *success)
{
    struct TBEntry *ptr;
    struct TBHashEntry *ptr2;
    uint64_t idx;
    uint64_t key;
    int i;
    uint8_t res;
    int p[TBPIECES];

    // Obtain the position's material signature key.
    key = calc_key(pos, false);

    // Test for KvK.
    if (key == KEY_KvK)
        return 0;

    ptr2 = TB_hash[key >> (64 - TBHASHBITS)];
    for (i = 0; i < HSHMAX; i++)
    {
        if (ptr2[i].key == key)
            break;
    }
    if (i == HSHMAX)
    {
        *success = 0;
        return 0;
    }

    ptr = ptr2[i].ptr;
    if (!ptr->ready)
    {
        LOCK(TB_MUTEX);
        if (!ptr->ready)
        {
            char str[16];
            prt_str(pos, str, ptr->key != key);
            if (!init_table_wdl(ptr, str))
            {
                ptr2[i].key = 0ULL;
                *success = 0;
                UNLOCK(TB_MUTEX);
                return 0;
            }
            // Memory barrier to ensure ptr->ready = 1 is not reordered.
#ifdef __GNUC__
            __asm__ __volatile__ ("" ::: "memory");
#elif defined(_MSC_VER)
            MemoryBarrier();
#endif
            ptr->ready = 1;
        }
        UNLOCK(TB_MUTEX);
    }

    int bside, mirror, cmirror;
    if (!ptr->symmetric)
    {
        if (key != ptr->key)
        {
            cmirror = 8;
            mirror = 0x38;
            bside = pos->turn;
        }
        else
        {
            cmirror = mirror = 0;
            bside = !pos->turn;
        }
    }
    else
    {
        cmirror = (pos->turn? 0: 8);
        mirror = (pos->turn? 0: 0x38);
        bside = 0;
    }

    // p[i] is to contain the square 0-63 (A1-H8) for a piece of type
    // pc[i] ^ cmirror, where 1 = white pawn, ..., 14 = black king.
    // Pieces of the same type are guaranteed to be consecutive.
    if (!ptr->has_pawns)
    {
        struct TBEntry_piece *entry = (struct TBEntry_piece *)ptr;
        uint8_t *pc = entry->pieces[bside];
        for (i = 0; i < entry->num; )
        {
            uint64_t bb = get_pieces(pos, pc[i] ^ cmirror);
            do
            {
                p[i++] = lsb(bb);
                bb = poplsb(bb);
            } while (bb);
        }
        idx = encode_piece(entry, entry->norm[bside], p, entry->factor[bside]);
        res = decompress_pairs(entry->precomp[bside], idx);
    }
    else
    {
        struct TBEntry_pawn *entry = (struct TBEntry_pawn *)ptr;
        int k = entry->file[0].pieces[0][0] ^ cmirror;
        uint64_t bb = get_pieces(pos, k);
        i = 0;
        do {
            p[i++] = lsb(bb) ^ mirror;
            bb = poplsb(bb);
        } while (bb);
        int f = pawn_file(entry, p);
        uint8_t *pc = entry->file[f].pieces[bside];
        for (; i < entry->num; )
        {
            bb = get_pieces(pos, pc[i] ^ cmirror);
            do
            {
                p[i++] = lsb(bb) ^ mirror;
                bb = poplsb(bb);
            } while (bb);
        }
        idx = encode_pawn(entry, entry->file[f].norm[bside], p,
            entry->file[f].factor[bside]);
        res = decompress_pairs(entry->file[f].precomp[bside], idx);
    }

    return ((int)res) - 2;
}

static int probe_dtz_table(const struct pos *pos, int wdl, int *success)
{
    struct TBEntry *ptr;
    uint64_t idx;
    int i, res;
    int p[TBPIECES];

    // Obtain the position's material signature key.
    uint64_t key = calc_key(pos, false);

    if (DTZ_table[0].key1 != key && DTZ_table[0].key2 != key)
    {
        for (i = 1; i < DTZ_ENTRIES; i++)
        {
            if (DTZ_table[i].key1 == key)
                break;
        }
        if (i < DTZ_ENTRIES)
        {
            struct DTZTableEntry table_entry = DTZ_table[i];
            for (; i > 0; i--)
                DTZ_table[i] = DTZ_table[i - 1];
            DTZ_table[0] = table_entry;
        }
        else
        {
            struct TBHashEntry *ptr2 = TB_hash[key >> (64 - TBHASHBITS)];
            for (i = 0; i < HSHMAX; i++)
            {
                if (ptr2[i].key == key)
                    break;
            }
            if (i == HSHMAX)
            {
                *success = 0;
                return 0;
            }
            ptr = ptr2[i].ptr;
            char str[16];
            int mirror = (ptr->key != key);
            prt_str(pos, str, mirror);
            if (DTZ_table[DTZ_ENTRIES - 1].entry)
                free_dtz_entry(DTZ_table[DTZ_ENTRIES-1].entry);
            for (i = DTZ_ENTRIES - 1; i > 0; i--)
                DTZ_table[i] = DTZ_table[i - 1];
            uint64_t key1 = calc_key(pos, mirror);
            uint64_t key2 = calc_key(pos, !mirror);
            load_dtz_table(str, key1, key2);
        }
    }

    ptr = DTZ_table[0].entry;
    if (!ptr)
    {
        *success = 0;
        return 0;
    }

    int bside, mirror, cmirror;
    if (!ptr->symmetric)
    {
        if (key != ptr->key)
        {
            cmirror = 8;
            mirror = 0x38;
            bside = pos->turn;
        }
        else
        {
            cmirror = mirror = 0;
            bside = !pos->turn;
        }
    }
    else
    {
        cmirror = (pos->turn? 0: 8);
        mirror = (pos->turn? 0: 0x38);
        bside = 0;
    }

    if (!ptr->has_pawns)
    {
        struct DTZEntry_piece *entry = (struct DTZEntry_piece *)ptr;
        if ((entry->flags & 1) != bside && !entry->symmetric)
        {
            *success = -1;
            return 0;
        }
        uint8_t *pc = entry->pieces;
        for (i = 0; i < entry->num;)
        {
            uint64_t bb = get_pieces(pos, pc[i] ^ cmirror);
            do
            {
                p[i++] = lsb(bb);
                bb = poplsb(bb);
            }
            while (bb);
        }
        idx = encode_piece((struct TBEntry_piece *)entry, entry->norm, p,
            entry->factor);
        res = decompress_pairs(entry->precomp, idx);

        if (entry->flags & 2)
            res = entry->map[entry->map_idx[wdl_to_map[wdl + 2]] + res];
        if (!(entry->flags & pa_flags[wdl + 2]) || (wdl & 1))
            res *= 2;
    }
    else
    {
        struct DTZEntry_pawn *entry = (struct DTZEntry_pawn *)ptr;
        int k = entry->file[0].pieces[0] ^ cmirror;
        uint64_t bb = get_pieces(pos, k);
        i = 0;
        do
        {
            p[i++] = lsb(bb) ^ mirror;
            bb = poplsb(bb);
        }
        while (bb);
        int f = pawn_file((struct TBEntry_pawn *)entry, p);
        if ((entry->flags[f] & 1) != bside)
        {
            *success = -1;
            return 0;
        }
        uint8_t *pc = entry->file[f].pieces;
        for (; i < entry->num;)
        {
            bb = get_pieces(pos, pc[i] ^ cmirror);
            do
            {
                p[i++] = lsb(bb) ^ mirror;
                bb = poplsb(bb);
            }
            while (bb);
        }
        idx = encode_pawn((struct TBEntry_pawn *)entry, entry->file[f].norm,
            p, entry->file[f].factor);
        res = decompress_pairs(entry->file[f].precomp, idx);

        if (entry->flags[f] & 2)
            res = entry->map[entry->map_idx[f][wdl_to_map[wdl + 2]] + res];
        if (!(entry->flags[f] & pa_flags[wdl + 2]) || (wdl & 1))
            res *= 2;
    }

    return res;
}

static uint16_t *add_move(uint16_t *moves, bool promotes, unsigned from,
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
 * Generate all captures or promotions.
 */
static uint16_t *gen_captures_or_promotions(const struct pos *pos,
    uint16_t *moves)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->white: pos->black),
             them = (pos->turn? pos->black: pos->white);
    uint64_t b, att;
    {
        unsigned from = lsb(pos->kings & us);
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
        if (pos->turn && rank(from) == 6)
        {
            unsigned to = from + 8;
            if ((board(to) & occ) == 0)
                moves = add_move(moves, true, from, to);
        }
        else if (!pos->turn && rank(from) == 1)
        {
            unsigned to = from - 8;
            if ((board(to) & occ) == 0)
                moves = add_move(moves, true, from, to);
        }
    }
    return moves;
}

/*
 * Generate all non-capture pawn moves and promotions.
 */
static uint16_t *gen_pawn_quiets_or_promotions(const struct pos *pos,
    uint16_t *moves)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->white: pos->black);
    uint64_t b, att;

    for (b = us & pos->pawns; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        unsigned next = from + (pos->turn? 8: -8);
        att = 0;
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
 * Generate all en passant captures.
 */
static uint16_t *gen_pawn_ep_captures(const struct pos *pos, uint16_t *moves)
{
    if (pos->ep == 0)
        return moves;
    uint64_t ep = board(pos->ep);
    unsigned to = pos->ep;
    uint64_t us = (pos->turn? pos->white: pos->black);
    uint64_t b;
    for (b = us & pos->pawns; b; b = poplsb(b))
    {
        unsigned from = lsb(b);
        if ((pawn_attacks(from, pos->turn) & ep) != 0)
            moves = add_move(moves, false, from, to);
    }
    return moves;
}

/*
 * Generate all moves.
 */
static uint16_t *gen_moves(const struct pos *pos, uint16_t *moves)
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
static bool is_en_passant(const struct pos *pos, uint16_t move)
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
 * Test if the given position is legal.
 * (Pawns on backrank? Can the king be captured?)
 */
static bool is_legal(const struct pos *pos)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->black: pos->white),
             them = (pos->turn? pos->white: pos->black);
    uint64_t king = pos->kings & us;
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
static bool is_check(const struct pos *pos)
{
    uint64_t occ = pos->white | pos->black;
    uint64_t us = (pos->turn? pos->white: pos->black),
             them = (pos->turn? pos->black: pos->white);
    uint64_t king = pos->kings & us;
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
 * Test if the king is in checkmate.
 */
static bool is_mate(const struct pos *pos)
{
    if (!is_check(pos))
        return false;
    uint16_t moves0[MAX_MOVES];
    uint16_t *moves = moves0;
    uint16_t *end = gen_moves(pos, moves);
    for (; moves < end; moves++)
    {
        struct pos pos1;
        if (do_move(&pos1, pos, *moves))
            return false;
    }
    return true;
}

/*
 * Test if the position is valid.
 */
static bool is_valid(const struct pos *pos)
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

static bool do_move(struct pos *pos, const struct pos *pos0, uint16_t move)
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

static int probe_ab(const struct pos *pos, int alpha, int beta, int *success)
{
    int v;
    uint16_t moves0[64];
    uint16_t *moves = moves0;
    uint16_t *end = gen_captures_or_promotions(pos, moves);
    for (; moves < end; moves++)
    {
        if (is_en_passant(pos, *moves))
            continue;
        struct pos pos1;
        if (!do_move(&pos1, pos, *moves))
            continue;
        v = -probe_ab(&pos1, -beta, -alpha, success);
        if (*success == 0) 
            return 0;
        if (v > alpha)
        { 
            if (v >= beta) 
            { 
                *success = 2;
                return v;
            } 
            alpha = v;
        } 
    }

    v = probe_wdl_table(pos, success);
    if (*success == 0)
        return 0;
    if (alpha >= v)
    {
        *success = 1 + (alpha > 0);
        return alpha;
    }
    else
    {
        *success = 1;
        return v;
    }
}

static int probe_wdl(const struct pos *pos, int *success)
{
    *success = 1;
    int v = probe_ab(pos, -2, 2, success);
    if (*success == 0)
        return 0;

    // If en passant is not possible, we are done.
    if (pos->ep == 0)
        return v;

    // Now handle en passant.
    int v1 = -3;
    uint16_t moves0[2];      // Max=2 possible en-passant captures.
    uint16_t *moves = moves0;
    uint16_t *end = gen_pawn_ep_captures(pos, moves);
    for (; moves < end; moves++)
    {
        struct pos pos1;
        if (!do_move(&pos1, pos, *moves))
            continue;
        int v0 = -probe_ab(&pos1, -2, 2, success);
        if (*success == 0)
            return 0;
        if (v0 > v1)
            v1 = v0;
    }
    if (v1 > -3)
    {
        if (v1 >= v)
            v = v1;
        else if (v == 0)
        {
            // Check whether there is at least one legal non-ep move.
            uint16_t moves0[MAX_MOVES];
            uint16_t *moves = moves0;
            uint16_t *end = gen_moves(pos, moves);
            bool found = false;
            for (; moves < end; moves++)
            {
                if (is_en_passant(pos, *moves))
                    continue;
                struct pos pos1;
                if (do_move(&pos1, pos, *moves))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                v = v1;     // Forced to play the losing ep capture.
        }
    }

    return v;
}

static int probe_dtz_no_ep(const struct pos *pos, int *success)
{
    int wdl, dtz;
    wdl = probe_ab(pos, -2, 2, success);
    if (wdl == 0)
        return 0;
    if (*success == 2)
        return wdl == 2 ? 1 : 101;

    uint16_t moves0[MAX_MOVES];
    uint16_t *moves = moves0, *end = NULL;
 
    if (wdl > 0)
    {
        // Generate at least all legal non-capturing pawn moves
        // including non-capturing promotions.
        end = gen_pawn_quiets_or_promotions(pos, moves);
        for (; moves < end; moves++)
        {
            struct pos pos1;
            if (!do_move(&pos1, pos, *moves))
                continue;
            int v = (pos1.ep == 0?
                -probe_ab(&pos1, -2, -wdl + 1, success):
                -probe_wdl(&pos1, success));
            if (*success == 0)
                return 0;
            if (v == wdl)
                return (v == 2? 1: 101);
        }
    }

    dtz = 1 + probe_dtz_table(pos, wdl, success);
    if (*success >= 0)
    {
        if (wdl & 1)
            dtz += 100;
        return (wdl >= 0? dtz: -dtz);
    }
    
    if (wdl > 0)
    {
        int best = BEST_NONE;
        moves = moves0;
        end = gen_moves(pos, moves);
        for (; moves < end; moves++)
        {
            struct pos pos1;
            if (!do_move(&pos1, pos, *moves))
                continue;
            if (pos1.rule50 == 0)
                continue;
            int v = -probe_dtz(&pos1, success);
            if (*success == 0)
                return 0;
            if (v > 0 && v + 1 < best)
                best = v + 1;
        }
        assert(best != BEST_NONE);
        return best;
    }
    else
    {
        int best = -1;
        end = gen_moves(pos, moves);
        for (; moves < end; moves++)
        {
            int v;
            struct pos pos1;
            if (!do_move(&pos1, pos, *moves))
                continue;
            if (pos1.rule50 == 0)
            {
                if (wdl == -2)
                    v = -1;
                else
                {
                    v = probe_ab(&pos1, 1, 2, success);
                    v = (v == 2) ? 0 : -101;
                }
            }
            else
                v = -probe_dtz(&pos1, success) - 1;
            if (*success == 0)
                return 0;
            if (v < best)
                best = v;
        }
        return best;
    }
}

static const int wdl_to_dtz[] =
{
    -1, -101, 0, 101, 1
};

/*
 * Probe the DTZ table for a particular position.
 * If *success != 0, the probe was successful.
 * The return value is from the point of view of the side to move:
 *         n < -100 : loss, but draw under 50-move rule
 * -100 <= n < -1   : loss in n ply (assuming 50-move counter == 0)
 *         0            : draw
 *     1 < n <= 100 : win in n ply (assuming 50-move counter == 0)
 *   100 < n        : win, but draw under 50-move rule
 *
 * The return value n can be off by 1: a return value -n can mean a loss
 * in n+1 ply and a return value +n can mean a win in n+1 ply. This
 * cannot happen for tables with positions exactly on the "edge" of
 * the 50-move rule.
 *
 * This implies that if dtz > 0 is returned, the position is certainly
 * a win if dtz + 50-move-counter <= 99. Care must be taken that the engine
 * picks moves that preserve dtz + 50-move-counter <= 99.
 *
 * If n = 100 immediately after a capture or pawn move, then the position
 * is also certainly a win, and during the whole phase until the next
 * capture or pawn move, the inequality to be preserved is
 * dtz + 50-movecounter <= 100.
 *
 * In short, if a move is available resulting in dtz + 50-move-counter <= 99,
 * then do not accept moves leading to dtz + 50-move-counter == 100.
 */
static int probe_dtz(const struct pos *pos, int *success)
{
    *success = 1;
    int v = probe_dtz_no_ep(pos, success);
    if (*success == 0)
        return 0;

    if (pos->ep == 0)
        return v;

    int v1 = -3;
    uint16_t moves0[2];      // Max=2 possible en-passant captures.
    uint16_t *moves = moves0;
    uint16_t *end = gen_pawn_ep_captures(pos, moves);
    for (; moves < end; moves++)
    {
        struct pos pos1;
        if (!do_move(&pos1, pos, *moves))
            continue;
        int v0 = -probe_ab(&pos1, -2, 2, success);
        if (*success == 0)
            return 0;
        if (v0 > v1)
            v1 = v0;
    }

    if (v1 > -3)
    {
        v1 = wdl_to_dtz[v1 + 2];
        if (v < -100)
        {
            if (v1 >= 0)
                v = v1;
        }
        else if (v < 0)
        {
            if (v1 >= 0 || v1 < -100)
                v = v1;
        }
        else if (v > 100)
        {
            if (v1 > 0)
                v = v1;
        }
        else if (v > 0)
        {
            if (v1 == 1)
                v = v1;
        }
        else if (v1 >= 0)
            v = v1;
        else
        {
            uint16_t moves0[MAX_MOVES];
            uint16_t *moves = moves0;
            uint16_t *end = gen_moves(pos, moves);
            bool found = false;
            for (; moves < end; moves++)
            {
                if (is_en_passant(pos, *moves))
                    continue;
                struct pos pos1;
                if (do_move(&pos1, pos, *moves))
                {
                    found = true;
                    break;
                }
            }
            if (!found)
                v = v1;     // Forced to play the losing ep capture.
        }    
    }

    return v;
}

static unsigned dtz_to_wdl(int cnt50, int dtz)
{
    int wdl = 0;
    if (dtz > 0)
        wdl = (dtz + cnt50 <= 100? 2: 1);
    else if (dtz < 0)
        wdl = (-dtz + cnt50 <= 100? -2: -1);
    return wdl + 2;
}

static uint16_t probe_root(const struct pos *pos, int *score,
    unsigned *results)
{
    int success;
    int dtz = probe_dtz(pos, &success);
    if (!success)
        return 0;

    int16_t scores[MAX_MOVES];
    uint16_t moves0[MAX_MOVES];
    uint16_t *moves = moves0;
    uint16_t *end = gen_moves(pos, moves);
    size_t len = end - moves;
    size_t num_draw = 0;
    unsigned j = 0;
    for (unsigned i = 0; i < len; i++)
    {
        struct pos pos1;
        if (!do_move(&pos1, pos, moves[i]))
        {
            scores[i] = SCORE_ILLEGAL;
            continue;
        }
        int v = 0;
        if (dtz > 0 && is_mate(&pos1))
            v = 1;
        else
        {
            if (pos1.rule50 != 0)
            {
                v = -probe_dtz(&pos1, &success);
                if (v > 0)
                    v++;
                else if (v < 0)
                    v--;
            }
            else
            {
                v = -probe_wdl(&pos1, &success);
                v = wdl_to_dtz[v + 2];
            }
        }
        num_draw += (v == 0);
        if (!success)
            return 0;
        scores[i] = v;
        if (results != NULL)
        {
            unsigned res = 0;
            res = TB_SET_WDL(res, dtz_to_wdl(pos->rule50, v));
            res = TB_SET_FROM(res, move_from(moves[i]));
            res = TB_SET_TO(res, move_to(moves[i]));
            res = TB_SET_PROMOTES(res, move_promotes(moves[i]));
            res = TB_SET_EP(res, is_en_passant(pos, moves[i]));
            res = TB_SET_DTZ(res, (v < 0? -v: v));
            results[j++] = res;
        }
    }
    if (results != NULL)
        results[j++] = TB_RESULT_FAILED;
    if (score != NULL)
        *score = dtz;

    // Now be a bit smart about filtering out moves.
    if (dtz > 0)        // winning (or 50-move rule draw)
    {
        int best = BEST_NONE;
        uint16_t best_move = 0;
        for (unsigned i = 0; i < len; i++)
        {
            int v = scores[i];
            if (v == SCORE_ILLEGAL)
                continue;
            if (v > 0 && v < best)
            {
                best = v;
                best_move = moves[i];
            }
        }
        return (best == BEST_NONE? 0: best_move);
    }
    else if (dtz < 0)   // losing (or 50-move rule draw)
    {
        int best = 0;
        uint16_t best_move = 0;
        for (unsigned i = 0; i < len; i++)
        {
            int v = scores[i];
            if (v == SCORE_ILLEGAL)
                continue;
            if (v < best)
            {
                best = v;
                best_move = moves[i];
            }
        }
        return (best == 0? MOVE_CHECKMATE: best_move);
    }
    else                // drawing
    {
        // Check for stalemate:
        if (num_draw == 0)
            return MOVE_STALEMATE;

        // Select a "random" move that preserves the draw.
        // Uses calc_key as the PRNG.
        size_t count = calc_key(pos, !pos->turn) % num_draw;
        for (unsigned i = 0; i < len; i++)
        {
            int v = scores[i];
            if (v == SCORE_ILLEGAL)
                continue;
            if (v == 0)
            {
                if (count == 0)
                    return moves[i];
                count--;
            }
        }
        return 0;
    }
}

bool tb_init_impl(const char *path)
{
    if (sizeof(uint64_t) != 8 &&        // Paranoid check
            sizeof(uint32_t) != 4 &&
            sizeof(uint16_t) != 2 &&
            sizeof(uint8_t) != 1)
        return false;
    king_attacks_init();
    knight_attacks_init();
    bishop_attacks_init();
    rook_attacks_init();
    pawn_attacks_init();
    if (path == NULL)
        path = "";
    init_tablebases(path);
    return true;
}

unsigned tb_probe_wdl_impl(
    uint64_t white,
    uint64_t black,
    uint64_t kings,
    uint64_t queens,
    uint64_t rooks,
    uint64_t bishops,
    uint64_t knights,
    uint64_t pawns,
    unsigned ep,
    bool turn)
{
    struct pos pos =
    {
        white,
        black,
        kings,
        queens,
        rooks,
        bishops,
        knights,
        pawns,
        0,
        (uint8_t)ep,
        turn
    };
    int success;
    int v = probe_wdl(&pos, &success);
    if (success == 0)
        return TB_RESULT_FAILED;
    return (unsigned)(v + 2);
}

unsigned tb_probe_root_impl(
    uint64_t white,
    uint64_t black,
    uint64_t kings,
    uint64_t queens,
    uint64_t rooks,
    uint64_t bishops,
    uint64_t knights,
    uint64_t pawns,
    unsigned rule50,
    unsigned ep,
    bool turn,
    unsigned *results)
{
    struct pos pos =
    {
        white,
        black,
        kings,
        queens,
        rooks,
        bishops,
        knights,
        pawns,
        (uint8_t)rule50,
        (uint8_t)ep,
        turn
    };
    int dtz;
    if (!is_valid(&pos))
        return TB_RESULT_FAILED;
    uint16_t move = probe_root(&pos, &dtz, results);
    if (move == 0)
        return TB_RESULT_FAILED;
    if (move == MOVE_CHECKMATE)
        return TB_RESULT_CHECKMATE;
    if (move == MOVE_STALEMATE)
        return TB_RESULT_STALEMATE;
    unsigned res = 0;
    res = TB_SET_WDL(res, dtz_to_wdl(rule50, dtz));
    res = TB_SET_DTZ(res, (dtz < 0? -dtz: dtz));
    res = TB_SET_FROM(res, move_from(move));
    res = TB_SET_TO(res, move_to(move));
    res = TB_SET_PROMOTES(res, move_promotes(move));
    res = TB_SET_EP(res, is_en_passant(&pos, move));
    return res;
}

#ifndef TB_NO_HELPER_API

unsigned tb_pop_count(uint64_t bb)
{
    return popcount(bb);
}

unsigned tb_lsb(uint64_t bb)
{
    return lsb(bb);
}

uint64_t tb_pop_lsb(uint64_t bb)
{
    return poplsb(bb);
}

uint64_t tb_king_attacks(unsigned sq)
{
    return king_attacks(sq);
}

uint64_t tb_queen_attacks(unsigned sq, uint64_t occ)
{
    return queen_attacks(sq, occ);
}

uint64_t tb_rook_attacks(unsigned sq, uint64_t occ)
{
    return rook_attacks(sq, occ);
}

uint64_t tb_bishop_attacks(unsigned sq, uint64_t occ)
{
    return bishop_attacks(sq, occ);
}

uint64_t tb_knight_attacks(unsigned sq)
{
    return knight_attacks(sq);
}

uint64_t tb_pawn_attacks(unsigned sq, bool color)
{
    return pawn_attacks(sq, color);
}

#endif      /* TB_NO_HELPER_API */

