/*
 * tbconfig.h
 * (C) 2015 basil, all rights reserved,
 *
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

#ifndef TBCONFIG_H
#define TBCONFIG_H

/****************************************************************************/
/* BUILD CONFIG:                                                            */
/****************************************************************************/

/*
 * Define TB_CUSTOM_POP_COUNT to override the internal popcount
 * implementation. To do this supply a macro or function definition
 * here:
 */
/* #define TB_CUSTOM_POP_COUNT(x) <DEFINITION> */

/*
 * Define TB_CUSTOM_LSB to override the internal lsb
 * implementation. To do this supply a macro or function definition
 * here:
 */
/* #define TB_CUSTOM_LSB(x) <DEFINITION> */

/*
 * Define TB_CUSTOM_BSWAP32 to override the internal bswap32
 * implementation. To do this supply a macro or function definition
 * here:
 */
/* #define TB_CUSTOM_BSWAP32(x) <DEFINITION> */

/*
 * Define TB_CUSTOM_BSWAP64 to override the internal bswap64
 * implementation. To do this supply a macro or function definition
 * here:
 */
/* #define TB_CUSTOM_BSWAP64(x) <DEFINITION> */

/*
 * Define TB_NO_STDINT if you do not want to use <stdint.h> or it is not
 * available.
 */
/* #define TB_NO_STDINT */

/*
 * Define TB_NO_STDBOOL if you do not want to use <stdbool.h> or it is not
 * available or unnecessary (e.g. C++).
 */
#define TB_NO_STDBOOL

/*
 * Define TB_NO_THREADS if your program is not multi-threaded.
 */
/* #define TB_NO_THREADS */

/*
 * Define TB_NO_HELPER_API if you do not need the helper API.
 */
#define TB_NO_HELPER_API

/*
 * Define TB_NO_HW_POP_COUNT if there is no hardware popcount instruction.
 */
#define TB_NO_HW_POP_COUNT

/***************************************************************************/
/* ENGINE INTEGRATION CONFIG                                               */
/***************************************************************************/

/*
 * If you are integrating tbprobe into an engine, you can replace some of
 * tbprobe's built-in functionality with that already provided by the engine.
 * This is OPTIONAL.  If no definition are provided then tbprobe will use its
 * own internal defaults.  That said, for engines it is generally a good idea
 * to avoid redundancy.
 */

/*
 * Define TB_KING_ATTACKS(square) to return the king attacks bitboard for a
 * king at `square'.
 */
/* #define TB_KING_ATTACKS(square)          <DEFINITION> */

/*
 * Define TB_KNIGHT_ATTACKS(square) to return the knight attacks bitboard for
 * a knight at `square'.
 */
/* #define TB_KNIGHT_ATTACKS(square)        <DEFINITION> */

/*
 * Define TB_ROOK_ATTACKS(square, occ) to return the rook attacks bitboard
 * for a rook at `square' assuming the given `occ' occupancy bitboard.
 */
/* #define TB_ROOK_ATTACKS(square, occ)     <DEFINITION> */

/*
 * Define TB_BISHOP_ATTACKS(square, occ) to return the bishop attacks bitboard
 * for a bishop at `square' assuming the given `occ' occupancy bitboard.
 */
/* #define TB_BISHOP_ATTACKS(square, occ)   <DEFINITION> */

/*
 * Define TB_QUEEN_ATTACKS(square, occ) to return the queen attacks bitboard
 * for a queen at `square' assuming the given `occ' occupancy bitboard.
 * NOTE: If no definition is provided then tbprobe will use:
 *       TB_ROOK_ATTACKS(square, occ) | TB_BISHOP_ATTACKS(square, occ)
 */
/* #define TB_QUEEN_ATTACKS(square, occ)    <DEFINITION> */

/*
 * Define TB_PAWN_ATTACKS(square, color) to return the pawn attacks bitboard
 * for a `color' pawn at `square'.
 * NOTE: This definition must work for pawns on ranks 1 and 8.  For example,
 *       a white pawn on e1 attacks d2 and f2.  A black pawn on e1 attacks
 *       nothing.  Etc.
 * NOTE: This definition must not include en passant captures.
 */
/* #define TB_PAWN_ATTACKS(square, color)   <DEFINITION> */

#endif
