/**
 *magicmoves.h
 *
 *Header file for magic move bitboard generation.  Include this in any files
 *need this functionality.
 *
 *Usage:
 *You must first initialize the generator with a call to initmagicmoves().
 *Then you can use the following macros for generating move bitboards by
 *giving them a square and an occupancy.  The macro will then "return"
 *the correct move bitboard for that particular square and occupancy. It
 *has been named R_MAGIC and B_MAGIC so that it will not conflict with
 *any functions/macros in your chess program called Rmoves/Bmoves. You
 *can macro B_MAGIC/R_MAGIC to Bmoves/Rmoves if you wish.  If you want to
 *minimize the size of the bitboards, make MINIMIZE_MAGIC uncommented in this
 *header (more info on this later).  Where you typedef your unsigned 64-bit
 *integer declare __64_BIT_INTEGER_DEFINED__.  If USE_INLINING is uncommented,
 *the macros will be expressed as inlined functions.  If PERFECT_MAGIC_HASH is
 *uncomment, the move generator will use an additional indrection to make the
 *table sizes smaller : (~50kb+((original size)/sizeof(PERFECT_MAGIC_HASH)).
 *The size listed from here on out are the sizes without PERFECT_MAGIC_HASH.
 *
 *B_MAGIC(square, occupancy)
 *R_MAGIC(square, occupancy)
 *
 *Square is an integer that is greater than or equal to zero and less than 64.
 *Occupancy is any unsigned 64-bit integer that describes which squares on
 *the board are occupied.
 *
 *The following macros are identical to R_MAGIC and B_MAGIC except that the 
 *occupancy is assumed to already have been "masked".  Look at the following
 *source or read up on the internet about magic bitboard move generation to
 *understand the usage of these macros and what it means by "an occupancy that
 *has already been masked".  Using the following macros when possible might be
 *a tiny bit faster than using R_MAGIC and B_MAGIC because it avoids an array
 *access and a 64-bit & operation.
 *
 *BmagicNOMASK(square, occupancy)
 *RmagicNOMASK(square, occupancy)
 *
 *Unsigned 64 bit integers are referenced by this generator as U64.
 *Edit the beginning lines of this header for the defenition of a 64 bit
 *integer if necessary.
 *
 *If MINIMIZE_MAGIC is defined before including this file:
 *The move bitboard generator will use up 841kb of memory.
 *41kb of memory is used for the bishop database and 800kb is used for the rook
 *database.  If you feel the 800kb rook database is too big, then comment it out
 *and use a more traditional move bitboard generator in conjunction with the
 *magic move bitboard generator for bishops.
 *
 *If MINIMIAZE_MAGIC is not defined before including this file:
 *The move bitboard generator will use up 2304kb of memory but might perform a bit
 *faster.
 *
 *Copyright (C) 2006 Pradyumna Kannan.
 *
 *This code is provided 'as-is', without any express or implied warranty.
 *In no event will the authors be held liable for any damages arising from
 *the use of this code. Permission is granted to anyone to use this
 *code for any purpose, including commercial applications, and to alter
 *it and redistribute it freely, subject to the following restrictions:
 *
 *1. The origin of this code must not be misrepresented; you must not
 *claim that you wrote the original code. If you use this code in a
 *product, an acknowledgment in the product documentation would be
 *appreciated but is not required.
 *
 *2. Altered source versions must be plainly marked as such, and must not be
 *misrepresented as being the original code.
 *
 *3. This notice may not be removed or altered from any source distribution.
 */

#ifndef _magicmovesh
#define _magicmovesh

/*********MODIFY THE FOLLOWING IF NECESSARY********/
//the default configuration is the best

//Uncommont either one of the following or none
#define MINIMIZE_MAGIC
//#define PERFECT_MAGIC_HASH unsigned short

//the following works only for perfect magic hash or no defenitions above
//it uses variable shift for each square
//#define VARIABLE_SHIFT

#ifdef _MSC_VER
	#define USE_INLINING
#endif

#ifndef __64_BIT_INTEGER_DEFINED__
#define __64_BIT_INTEGER_DEFINED__
#if defined(_MSC_VER) && _MSC_VER<1300
	typedef unsigned __int64 U64; //For the old microsoft compilers
#else
	typedef unsigned long long  U64; //Supported by MSC 13.00+ and GCC
#endif //defined(_MSC_VER) && _MSC_VER<1300
#endif //__64_BIT_INTEGER_DEFINED__
/***********MODIFY THE ABOVE IF NECESSARY**********/

#ifndef C64
	#if (!defined(_MSC_VER) || _MSC_VER>1300)
		#define C64(constantU64) constantU64##ULL
	#else
		#define C64(constantU64) constantU64
	#endif
#endif

#ifdef USE_INLINING
	#ifdef _MSC_VER
		#define MAGICMOVES_INLINE_KEYWORD __forceinline
	#endif
	#ifdef __GNUC__
		#define MAGICMOVES_INLINE_KEYWORD __inline__
	#endif
	#ifndef MAGICMOVES_INLINE_KEYWORD
		#define MAGICMOVES_INLINE_KEYWORD inline
	#endif
#endif

extern const U64 magicmoves_r_magics[64];
extern const U64 magicmoves_r_mask[64];
extern const U64 magicmoves_b_magics[64];
extern const U64 magicmoves_b_mask[64];
extern const unsigned int magicmoves_b_shift[64];
extern const unsigned int magicmoves_r_shift[64];

#ifndef VARIABLE_SHIFT
	#define MINIMAL_B_BITS_SHIFT(square) 55
	#define MINIMAL_R_BITS_SHIFT(square) 52
#else
	#define MINIMAL_B_BITS_SHIFT(square) magicmoves_b_shift[square]
	#define MINIMAL_R_BITS_SHIFT(square) magicmoves_r_shift[square]
#endif



#ifndef PERFECT_MAGIC_HASH
	#ifdef MINIMIZE_MAGIC

		#ifndef USE_INLINING
			#define B_MAGIC(square, occupancy) *(magicmoves_b_indecies[square]+((((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>magicmoves_b_shift[square]))
			#define R_MAGIC(square, occupancy) *(magicmoves_r_indecies[square]+((((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>magicmoves_r_shift[square]))
			#define BmagicNOMASK(square, occupancy) *(magicmoves_b_indecies[square]+(((occupancy)*magicmoves_b_magics[square])>>magicmoves_b_shift[square]))
			#define RmagicNOMASK(square, occupancy) *(magicmoves_r_indecies[square]+(((occupancy)*magicmoves_r_magics[square])>>magicmoves_r_shift[square]))
		#endif //USE_INLINING

		//extern U64 magicmovesbdb[5248];
		extern const U64* magicmoves_b_indecies[64];

		//extern U64 magicmovesrdb[102400];
		extern const U64* magicmoves_r_indecies[64];

	#else //Don't Minimize database size

		#ifndef USE_INLINING
			#define B_MAGIC(square, occupancy) magicmovesbdb[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]
			#define R_MAGIC(square, occupancy) magicmovesrdb[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]
			#define BmagicNOMASK(square, occupancy) magicmovesbdb[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]
			#define RmagicNOMASK(square, occupancy) magicmovesrdb[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]
		#endif //USE_INLINING

		extern U64 magicmovesbdb[64][1<<9];
		extern U64 magicmovesrdb[64][1<<12];

	#endif //MINIMIAZE_MAGICMOVES
#else //PERFCT_MAGIC_HASH defined
	#ifndef MINIMIZE_MAGIC

		#ifndef USE_INLINING
			#define B_MAGIC(square, occupancy) magicmovesbdb[magicmoves_b_indecies[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]]
			#define R_MAGIC(square, occupancy) magicmovesrdb[magicmoves_r_indecies[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]]
			#define BmagicNOMASK(square, occupancy) magicmovesbdb[magicmoves_b_indecies[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]]
			#define RmagicNOMASK(square, occupancy) magicmovesrdb[magicmoves_r_indecies[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]]
		#endif //USE_INLINING

		extern U64 magicmovesbdb[1428];
		extern U64 magicmovesrdb[4900];
		extern PERFECT_MAGIC_HASH magicmoves_b_indecies[64][1<<9];
		extern PERFECT_MAGIC_HASH magicmoves_r_indecies[64][1<<12];
	#else
		#error magicmoves - MINIMIZED_MAGIC and PERFECT_MAGIC_HASH cannot be used together
	#endif
#endif //PERFCT_MAGIC_HASH

#ifdef USE_INLINING
	MAGICMOVES_INLINE_KEYWORD U64 B_MAGIC(const unsigned int square,const U64 occupancy)
	{
		#ifndef PERFECT_MAGIC_HASH
			#ifdef MINIMIZE_MAGIC
				return *(magicmoves_b_indecies[square]+(((occupancy&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>magicmoves_b_shift[square]));
			#else
				return magicmovesbdb[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)];
			#endif
		#else
			return magicmovesbdb[magicmoves_b_indecies[square][(((occupancy)&magicmoves_b_mask[square])*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]];
		#endif
	}
	MAGICMOVES_INLINE_KEYWORD U64 R_MAGIC(const unsigned int square,const U64 occupancy)
	{
		#ifndef PERFECT_MAGIC_HASH
			#ifdef MINIMIZE_MAGIC
				return *(magicmoves_r_indecies[square]+(((occupancy&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>magicmoves_r_shift[square]));
			#else
				return magicmovesrdb[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)];
			#endif
		#else
			return magicmovesrdb[magicmoves_r_indecies[square][(((occupancy)&magicmoves_r_mask[square])*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]];
		#endif
	}
	MAGICMOVES_INLINE_KEYWORD U64 BmagicNOMASK(const unsigned int square,const U64 occupancy)
	{
		#ifndef PERFECT_MAGIC_HASH
			#ifdef MINIMIZE_MAGIC
				return *(magicmoves_b_indecies[square]+(((occupancy)*magicmoves_b_magics[square])>>magicmoves_b_shift[square]));
			#else
				return magicmovesbdb[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)];
			#endif
		#else
			return magicmovesbdb[magicmoves_b_indecies[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]];
		#endif
	}
	MAGICMOVES_INLINE_KEYWORD U64 RmagicNOMASK(const unsigned int square, const U64 occupancy)
	{
		#ifndef PERFECT_MAGIC_HASH
			#ifdef MINIMIZE_MAGIC
				return *(magicmoves_r_indecies[square]+(((occupancy)*magicmoves_r_magics[square])>>magicmoves_r_shift[square]));
			#else
				return magicmovesrdb[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)];
			#endif
		#else
			return magicmovesrdb[magicmoves_r_indecies[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]];
		#endif
	}

	MAGICMOVES_INLINE_KEYWORD U64 Q_MAGIC(const unsigned int square,const U64 occupancy)
	{
		return B_MAGIC(square,occupancy)|R_MAGIC(square,occupancy);
	}
	MAGICMOVES_INLINE_KEYWORD U64 Q_MAGIC_NOMASK(const unsigned int square, const U64 occupancy)
	{
		return BmagicNOMASK(square,occupancy)|RmagicNOMASK(square,occupancy);
	}
#else //!USE_INLINING

#define Q_MAGIC(square, occupancy) (B_MAGIC(square,occupancy)|R_MAGIC(square,occupancy))
#define Q_MAGIC_NOMASK(square, occupancy) (BmagicNOMASK(square,occupancy)|RmagicNOMASK(square,occupancy))

#endif //USE_INLINING

void initmagicmoves(void);

#endif //_magicmovesh
