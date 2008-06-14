/**
 *magicmoves.h
 *
 *Source file for magic move bitboard generation.
 *
 *See header file for usage.
 *
 *The magic keys are not optimal for all squares but they are very close
 *to optimal.
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

#include "magicmoves.h"

#ifdef _MSC_VER
	#pragma message("MSC compatible compiler detected -- turning off warning 4312,4146")
	#pragma warning( disable : 4312)
	#pragma warning( disable : 4146)
#endif

//For rooks

//original 12 bit keys
//C64(0x0000002040810402 - H8 12 bit
//C64(0x0000102040800101 - A8 12 bit
//C64(0x0000102040008101 - B8 11 bit
//C64(0x0000081020004101 - C8 11 bit

//Adapted Grant Osborne's keys
//C64(0x0001FFFAABFAD1A2 - H8 11 bit
//C64(0x00FFFCDDFCED714A - A8 11 bit
//C64(0x007FFCDDFCED714A - B8 10 bit
//C64(0x003FFFCDFFD88096 - C8 10 bit

const unsigned int magicmoves_r_shift[64]=
{
	52, 53, 53, 53, 53, 53, 53, 52,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 54, 54, 54, 54, 53,
	53, 54, 54, 53, 53, 53, 53, 53
};

const U64 magicmoves_r_magics[64]=
{
	C64(0x0080001020400080), C64(0x0040001000200040), C64(0x0080081000200080), C64(0x0080040800100080),
	C64(0x0080020400080080), C64(0x0080010200040080), C64(0x0080008001000200), C64(0x0080002040800100),
	C64(0x0000800020400080), C64(0x0000400020005000), C64(0x0000801000200080), C64(0x0000800800100080),
	C64(0x0000800400080080), C64(0x0000800200040080), C64(0x0000800100020080), C64(0x0000800040800100),
	C64(0x0000208000400080), C64(0x0000404000201000), C64(0x0000808010002000), C64(0x0000808008001000),
	C64(0x0000808004000800), C64(0x0000808002000400), C64(0x0000010100020004), C64(0x0000020000408104),
	C64(0x0000208080004000), C64(0x0000200040005000), C64(0x0000100080200080), C64(0x0000080080100080),
	C64(0x0000040080080080), C64(0x0000020080040080), C64(0x0000010080800200), C64(0x0000800080004100),
	C64(0x0000204000800080), C64(0x0000200040401000), C64(0x0000100080802000), C64(0x0000080080801000),
	C64(0x0000040080800800), C64(0x0000020080800400), C64(0x0000020001010004), C64(0x0000800040800100),
	C64(0x0000204000808000), C64(0x0000200040008080), C64(0x0000100020008080), C64(0x0000080010008080),
	C64(0x0000040008008080), C64(0x0000020004008080), C64(0x0000010002008080), C64(0x0000004081020004),
	C64(0x0000204000800080), C64(0x0000200040008080), C64(0x0000100020008080), C64(0x0000080010008080),
	C64(0x0000040008008080), C64(0x0000020004008080), C64(0x0000800100020080), C64(0x0000800041000080),
	C64(0x00FFFCDDFCED714A), C64(0x007FFCDDFCED714A), C64(0x003FFFCDFFD88096), C64(0x0000040810002101),
	C64(0x0001000204080011), C64(0x0001000204000801), C64(0x0001000082000401), C64(0x0001FFFAABFAD1A2)
};
const U64 magicmoves_r_mask[64]=
{	
	C64(0x000101010101017E), C64(0x000202020202027C), C64(0x000404040404047A), C64(0x0008080808080876),
	C64(0x001010101010106E), C64(0x002020202020205E), C64(0x004040404040403E), C64(0x008080808080807E),
	C64(0x0001010101017E00), C64(0x0002020202027C00), C64(0x0004040404047A00), C64(0x0008080808087600),
	C64(0x0010101010106E00), C64(0x0020202020205E00), C64(0x0040404040403E00), C64(0x0080808080807E00),
	C64(0x00010101017E0100), C64(0x00020202027C0200), C64(0x00040404047A0400), C64(0x0008080808760800),
	C64(0x00101010106E1000), C64(0x00202020205E2000), C64(0x00404040403E4000), C64(0x00808080807E8000),
	C64(0x000101017E010100), C64(0x000202027C020200), C64(0x000404047A040400), C64(0x0008080876080800),
	C64(0x001010106E101000), C64(0x002020205E202000), C64(0x004040403E404000), C64(0x008080807E808000),
	C64(0x0001017E01010100), C64(0x0002027C02020200), C64(0x0004047A04040400), C64(0x0008087608080800),
	C64(0x0010106E10101000), C64(0x0020205E20202000), C64(0x0040403E40404000), C64(0x0080807E80808000),
	C64(0x00017E0101010100), C64(0x00027C0202020200), C64(0x00047A0404040400), C64(0x0008760808080800),
	C64(0x00106E1010101000), C64(0x00205E2020202000), C64(0x00403E4040404000), C64(0x00807E8080808000),
	C64(0x007E010101010100), C64(0x007C020202020200), C64(0x007A040404040400), C64(0x0076080808080800),
	C64(0x006E101010101000), C64(0x005E202020202000), C64(0x003E404040404000), C64(0x007E808080808000),
	C64(0x7E01010101010100), C64(0x7C02020202020200), C64(0x7A04040404040400), C64(0x7608080808080800),
	C64(0x6E10101010101000), C64(0x5E20202020202000), C64(0x3E40404040404000), C64(0x7E80808080808000)
};

//my original tables for bishops
const unsigned int magicmoves_b_shift[64]=
{
	58, 59, 59, 59, 59, 59, 59, 58,
	59, 59, 59, 59, 59, 59, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 55, 55, 57, 59, 59,
	59, 59, 57, 57, 57, 57, 59, 59,
	59, 59, 59, 59, 59, 59, 59, 59,
	58, 59, 59, 59, 59, 59, 59, 58
};

const U64 magicmoves_b_magics[64]=
{
	C64(0x0002020202020200), C64(0x0002020202020000), C64(0x0004010202000000), C64(0x0004040080000000),
	C64(0x0001104000000000), C64(0x0000821040000000), C64(0x0000410410400000), C64(0x0000104104104000),
	C64(0x0000040404040400), C64(0x0000020202020200), C64(0x0000040102020000), C64(0x0000040400800000),
	C64(0x0000011040000000), C64(0x0000008210400000), C64(0x0000004104104000), C64(0x0000002082082000),
	C64(0x0004000808080800), C64(0x0002000404040400), C64(0x0001000202020200), C64(0x0000800802004000),
	C64(0x0000800400A00000), C64(0x0000200100884000), C64(0x0000400082082000), C64(0x0000200041041000),
	C64(0x0002080010101000), C64(0x0001040008080800), C64(0x0000208004010400), C64(0x0000404004010200),
	C64(0x0000840000802000), C64(0x0000404002011000), C64(0x0000808001041000), C64(0x0000404000820800),
	C64(0x0001041000202000), C64(0x0000820800101000), C64(0x0000104400080800), C64(0x0000020080080080),
	C64(0x0000404040040100), C64(0x0000808100020100), C64(0x0001010100020800), C64(0x0000808080010400),
	C64(0x0000820820004000), C64(0x0000410410002000), C64(0x0000082088001000), C64(0x0000002011000800),
	C64(0x0000080100400400), C64(0x0001010101000200), C64(0x0002020202000400), C64(0x0001010101000200),
	C64(0x0000410410400000), C64(0x0000208208200000), C64(0x0000002084100000), C64(0x0000000020880000),
	C64(0x0000001002020000), C64(0x0000040408020000), C64(0x0004040404040000), C64(0x0002020202020000),
	C64(0x0000104104104000), C64(0x0000002082082000), C64(0x0000000020841000), C64(0x0000000000208800),
	C64(0x0000000010020200), C64(0x0000000404080200), C64(0x0000040404040400), C64(0x0002020202020200)
};


const U64 magicmoves_b_mask[64]=
{
	C64(0x0040201008040200), C64(0x0000402010080400), C64(0x0000004020100A00), C64(0x0000000040221400),
	C64(0x0000000002442800), C64(0x0000000204085000), C64(0x0000020408102000), C64(0x0002040810204000),
	C64(0x0020100804020000), C64(0x0040201008040000), C64(0x00004020100A0000), C64(0x0000004022140000),
	C64(0x0000000244280000), C64(0x0000020408500000), C64(0x0002040810200000), C64(0x0004081020400000),
	C64(0x0010080402000200), C64(0x0020100804000400), C64(0x004020100A000A00), C64(0x0000402214001400),
	C64(0x0000024428002800), C64(0x0002040850005000), C64(0x0004081020002000), C64(0x0008102040004000),
	C64(0x0008040200020400), C64(0x0010080400040800), C64(0x0020100A000A1000), C64(0x0040221400142200),
	C64(0x0002442800284400), C64(0x0004085000500800), C64(0x0008102000201000), C64(0x0010204000402000),
	C64(0x0004020002040800), C64(0x0008040004081000), C64(0x00100A000A102000), C64(0x0022140014224000),
	C64(0x0044280028440200), C64(0x0008500050080400), C64(0x0010200020100800), C64(0x0020400040201000),
	C64(0x0002000204081000), C64(0x0004000408102000), C64(0x000A000A10204000), C64(0x0014001422400000),
	C64(0x0028002844020000), C64(0x0050005008040200), C64(0x0020002010080400), C64(0x0040004020100800),
	C64(0x0000020408102000), C64(0x0000040810204000), C64(0x00000A1020400000), C64(0x0000142240000000),
	C64(0x0000284402000000), C64(0x0000500804020000), C64(0x0000201008040200), C64(0x0000402010080400),
	C64(0x0002040810204000), C64(0x0004081020400000), C64(0x000A102040000000), C64(0x0014224000000000),
	C64(0x0028440200000000), C64(0x0050080402000000), C64(0x0020100804020000), C64(0x0040201008040200)
};

#ifdef MINIMIZE_MAGIC
U64 magicmovesbdb[5248];
const U64* magicmoves_b_indecies[64]=
{
	magicmovesbdb+4992, magicmovesbdb+2624,  magicmovesbdb+256,  magicmovesbdb+896,
	magicmovesbdb+1280, magicmovesbdb+1664, magicmovesbdb+4800, magicmovesbdb+5120,
	magicmovesbdb+2560, magicmovesbdb+2656,  magicmovesbdb+288,  magicmovesbdb+928,
	magicmovesbdb+1312, magicmovesbdb+1696, magicmovesbdb+4832, magicmovesbdb+4928,
	magicmovesbdb+0,     magicmovesbdb+128,  magicmovesbdb+320,  magicmovesbdb+960,
	magicmovesbdb+1344, magicmovesbdb+1728, magicmovesbdb+2304, magicmovesbdb+2432,
	magicmovesbdb+32,    magicmovesbdb+160,  magicmovesbdb+448, magicmovesbdb+2752,
	magicmovesbdb+3776, magicmovesbdb+1856, magicmovesbdb+2336, magicmovesbdb+2464,
	magicmovesbdb+64,    magicmovesbdb+192,  magicmovesbdb+576, magicmovesbdb+3264,
	magicmovesbdb+4288, magicmovesbdb+1984, magicmovesbdb+2368, magicmovesbdb+2496,
	magicmovesbdb+96,    magicmovesbdb+224,  magicmovesbdb+704, magicmovesbdb+1088,
	magicmovesbdb+1472, magicmovesbdb+2112, magicmovesbdb+2400, magicmovesbdb+2528,
	magicmovesbdb+2592, magicmovesbdb+2688,  magicmovesbdb+832, magicmovesbdb+1216,
	magicmovesbdb+1600, magicmovesbdb+2240, magicmovesbdb+4864, magicmovesbdb+4960,
	magicmovesbdb+5056, magicmovesbdb+2720,  magicmovesbdb+864, magicmovesbdb+1248,
	magicmovesbdb+1632, magicmovesbdb+2272, magicmovesbdb+4896, magicmovesbdb+5184
};
#else
	#ifndef PERFECT_MAGIC_HASH
		U64 magicmovesbdb[64][1<<9];
	#else
		U64 magicmovesbdb[1428];
		PERFECT_MAGIC_HASH magicmoves_b_indecies[64][1<<9];
	#endif
#endif

#ifdef MINIMIZE_MAGIC
U64 magicmovesrdb[102400];
const U64* magicmoves_r_indecies[64]=
{
	magicmovesrdb+86016, magicmovesrdb+73728, magicmovesrdb+36864, magicmovesrdb+43008,
	magicmovesrdb+47104, magicmovesrdb+51200, magicmovesrdb+77824, magicmovesrdb+94208,
	magicmovesrdb+69632, magicmovesrdb+32768, magicmovesrdb+38912, magicmovesrdb+10240,
	magicmovesrdb+14336, magicmovesrdb+53248, magicmovesrdb+57344, magicmovesrdb+81920,
	magicmovesrdb+24576, magicmovesrdb+33792,  magicmovesrdb+6144, magicmovesrdb+11264,
	magicmovesrdb+15360, magicmovesrdb+18432, magicmovesrdb+58368, magicmovesrdb+61440,
	magicmovesrdb+26624,  magicmovesrdb+4096,  magicmovesrdb+7168,     magicmovesrdb+0,
	 magicmovesrdb+2048, magicmovesrdb+19456, magicmovesrdb+22528, magicmovesrdb+63488,
	magicmovesrdb+28672,  magicmovesrdb+5120,  magicmovesrdb+8192,  magicmovesrdb+1024,
	 magicmovesrdb+3072, magicmovesrdb+20480, magicmovesrdb+23552, magicmovesrdb+65536,
	magicmovesrdb+30720, magicmovesrdb+34816,  magicmovesrdb+9216, magicmovesrdb+12288,
	magicmovesrdb+16384, magicmovesrdb+21504, magicmovesrdb+59392, magicmovesrdb+67584,
	magicmovesrdb+71680, magicmovesrdb+35840, magicmovesrdb+39936, magicmovesrdb+13312,
	magicmovesrdb+17408, magicmovesrdb+54272, magicmovesrdb+60416, magicmovesrdb+83968,
	magicmovesrdb+90112, magicmovesrdb+75776, magicmovesrdb+40960, magicmovesrdb+45056,
	magicmovesrdb+49152, magicmovesrdb+55296, magicmovesrdb+79872, magicmovesrdb+98304
};
#else
	#ifndef PERFECT_MAGIC_HASH
		U64 magicmovesrdb[64][1<<12];
	#else
		U64 magicmovesrdb[4900];
		PERFECT_MAGIC_HASH magicmoves_r_indecies[64][1<<12];
	#endif
#endif

U64 initmagicmoves_occ(const int* squares, const int numSquares, const U64 linocc)
{
	int i;
	U64 ret=0;
	for(i=0;i<numSquares;i++)
		if(linocc&(((U64)(1))<<i)) ret|=(((U64)(1))<<squares[i]);
	return ret;
}

U64 initmagicmoves_Rmoves(const int square, const U64 occ)
{
	U64 ret=0;
	U64 bit;
	U64 rowbits=(((U64)0xFF)<<(8*(square/8)));
	
	bit=(((U64)(1))<<square);
	do
	{
		bit<<=8;
		ret|=bit;
	}while(bit && !(bit&occ));
	bit=(((U64)(1))<<square);
	do
	{
		bit>>=8;
		ret|=bit;
	}while(bit && !(bit&occ));
	bit=(((U64)(1))<<square);
	do
	{
		bit<<=1;
		if(bit&rowbits) ret|=bit;
		else break;
	}while(!(bit&occ));
	bit=(((U64)(1))<<square);
	do
	{
		bit>>=1;
		if(bit&rowbits) ret|=bit;
		else break;
	}while(!(bit&occ));
	return ret;
}

U64 initmagicmoves_Bmoves(const int square, const U64 occ)
{
	U64 ret=0;
	U64 bit;
	U64 bit2;
	U64 rowbits=(((U64)0xFF)<<(8*(square/8)));
	
	bit=(((U64)(1))<<square);
	bit2=bit;
	do
	{
		bit<<=8-1;
		bit2>>=1;
		if(bit2&rowbits) ret|=bit;
		else break;
	}while(bit && !(bit&occ));
	bit=(((U64)(1))<<square);
	bit2=bit;
	do
	{
		bit<<=8+1;
		bit2<<=1;
		if(bit2&rowbits) ret|=bit;
		else break;
	}while(bit && !(bit&occ));
	bit=(((U64)(1))<<square);
	bit2=bit;
	do
	{
		bit>>=8-1;
		bit2<<=1;
		if(bit2&rowbits) ret|=bit;
		else break;
	}while(bit && !(bit&occ));
	bit=(((U64)(1))<<square);
	bit2=bit;
	do
	{
		bit>>=8+1;
		bit2>>=1;
		if(bit2&rowbits) ret|=bit;
		else break;
	}while(bit && !(bit&occ));
	return ret;
}

//used so that the original indecies can be left as const so that the compiler can optimize better

#ifndef PERFECT_MAGIC_HASH
	#ifdef MINIMIZE_MAGIC
		#define BmagicNOMASK2(square, occupancy) *(magicmoves_b_indecies2[square]+(((occupancy)*magicmoves_b_magics[square])>>magicmoves_b_shift[square]))
		#define RmagicNOMASK2(square, occupancy) *(magicmoves_r_indecies2[square]+(((occupancy)*magicmoves_r_magics[square])>>magicmoves_r_shift[square]))
	#else
		#define BmagicNOMASK2(square, occupancy) magicmovesbdb[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT(square)]
		#define RmagicNOMASK2(square, occupancy) magicmovesrdb[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT(square)]
	#endif
/*#else
	#define BmagicNOMASK2(square, occupancy) magicmovesbdb[magicmoves_b_indecies[square][((occupancy)*magicmoves_b_magics[square])>>MINIMAL_B_BITS_SHIFT]]
	#define RmagicNOMASK2(square, occupancy) magicmovesrdb[magicmoves_r_indecies[square][((occupancy)*magicmoves_r_magics[square])>>MINIMAL_R_BITS_SHIFT]]
*/
#endif

void initmagicmoves(void)
{
	int i;

	//for bitscans :
	//initmagicmoves_bitpos64_database[(x*C64(0x07EDD5E59A4E28C2))>>58]
	int initmagicmoves_bitpos64_database[64]={
	63,  0, 58,  1, 59, 47, 53,  2,
	60, 39, 48, 27, 54, 33, 42,  3,
	61, 51, 37, 40, 49, 18, 28, 20,
	55, 30, 34, 11, 43, 14, 22,  4,
	62, 57, 46, 52, 38, 26, 32, 41,
	50, 36, 17, 19, 29, 10, 13, 21,
	56, 45, 25, 31, 35, 16,  9, 12,
	44, 24, 15,  8, 23,  7,  6,  5};

#ifdef MINIMIZE_MAGIC
	//identical to magicmove_x_indecies except without the const modifer
	U64* magicmoves_b_indecies2[64]=
	{
		magicmovesbdb+4992, magicmovesbdb+2624,  magicmovesbdb+256,  magicmovesbdb+896,
		magicmovesbdb+1280, magicmovesbdb+1664, magicmovesbdb+4800, magicmovesbdb+5120,
		magicmovesbdb+2560, magicmovesbdb+2656,  magicmovesbdb+288,  magicmovesbdb+928,
		magicmovesbdb+1312, magicmovesbdb+1696, magicmovesbdb+4832, magicmovesbdb+4928,
		magicmovesbdb+0,     magicmovesbdb+128,  magicmovesbdb+320,  magicmovesbdb+960,
		magicmovesbdb+1344, magicmovesbdb+1728, magicmovesbdb+2304, magicmovesbdb+2432,
		magicmovesbdb+32,    magicmovesbdb+160,  magicmovesbdb+448, magicmovesbdb+2752,
		magicmovesbdb+3776, magicmovesbdb+1856, magicmovesbdb+2336, magicmovesbdb+2464,
		magicmovesbdb+64,    magicmovesbdb+192,  magicmovesbdb+576, magicmovesbdb+3264,
		magicmovesbdb+4288, magicmovesbdb+1984, magicmovesbdb+2368, magicmovesbdb+2496,
		magicmovesbdb+96,    magicmovesbdb+224,  magicmovesbdb+704, magicmovesbdb+1088,
		magicmovesbdb+1472, magicmovesbdb+2112, magicmovesbdb+2400, magicmovesbdb+2528,
		magicmovesbdb+2592, magicmovesbdb+2688,  magicmovesbdb+832, magicmovesbdb+1216,
		magicmovesbdb+1600, magicmovesbdb+2240, magicmovesbdb+4864, magicmovesbdb+4960,
		magicmovesbdb+5056, magicmovesbdb+2720,  magicmovesbdb+864, magicmovesbdb+1248,
		magicmovesbdb+1632, magicmovesbdb+2272, magicmovesbdb+4896, magicmovesbdb+5184
	};
	U64* magicmoves_r_indecies2[64]=
	{
		magicmovesrdb+86016, magicmovesrdb+73728, magicmovesrdb+36864, magicmovesrdb+43008,
		magicmovesrdb+47104, magicmovesrdb+51200, magicmovesrdb+77824, magicmovesrdb+94208,
		magicmovesrdb+69632, magicmovesrdb+32768, magicmovesrdb+38912, magicmovesrdb+10240,
		magicmovesrdb+14336, magicmovesrdb+53248, magicmovesrdb+57344, magicmovesrdb+81920,
		magicmovesrdb+24576, magicmovesrdb+33792,  magicmovesrdb+6144, magicmovesrdb+11264,
		magicmovesrdb+15360, magicmovesrdb+18432, magicmovesrdb+58368, magicmovesrdb+61440,
		magicmovesrdb+26624,  magicmovesrdb+4096,  magicmovesrdb+7168,     magicmovesrdb+0,
		magicmovesrdb+2048,  magicmovesrdb+19456, magicmovesrdb+22528, magicmovesrdb+63488,
		magicmovesrdb+28672,  magicmovesrdb+5120,  magicmovesrdb+8192,  magicmovesrdb+1024,
		magicmovesrdb+3072,  magicmovesrdb+20480, magicmovesrdb+23552, magicmovesrdb+65536,
		magicmovesrdb+30720, magicmovesrdb+34816,  magicmovesrdb+9216, magicmovesrdb+12288,
		magicmovesrdb+16384, magicmovesrdb+21504, magicmovesrdb+59392, magicmovesrdb+67584,
		magicmovesrdb+71680, magicmovesrdb+35840, magicmovesrdb+39936, magicmovesrdb+13312,
		magicmovesrdb+17408, magicmovesrdb+54272, magicmovesrdb+60416, magicmovesrdb+83968,
		magicmovesrdb+90112, magicmovesrdb+75776, magicmovesrdb+40960, magicmovesrdb+45056,
		magicmovesrdb+49152, magicmovesrdb+55296, magicmovesrdb+79872, magicmovesrdb+98304
	};
#endif // MINIMIZE_MAGIC


#ifdef PERFECT_MAGIC_HASH
	for(i=0;i<1428;i++)
		magicmovesbdb[i]=0;
	for(i=0;i<4900;i++)
		magicmovesrdb[i]=0;
#endif

	for(i=0;i<64;i++)
	{
		int squares[64];
		int numsquares=0;
		U64 temp=magicmoves_b_mask[i];
		while(temp)
		{
			U64 bit=temp&-temp;
			squares[numsquares++]=initmagicmoves_bitpos64_database[(bit*C64(0x07EDD5E59A4E28C2))>>58];
			temp^=bit;
		}
		for(temp=0;temp<(((U64)(1))<<numsquares);temp++)
		{
			U64 tempocc=initmagicmoves_occ(squares,numsquares,temp);
			#ifndef PERFECT_MAGIC_HASH
				BmagicNOMASK2(i,tempocc)=initmagicmoves_Bmoves(i,tempocc);
			#else
				U64 moves=initmagicmoves_Bmoves(i,tempocc);
				U64 index=(((tempocc)*magicmoves_b_magics[i])>>MINIMAL_B_BITS_SHIFT);
				int j;
				for(j=0;j<1428;j++)
				{
					if(!magicmovesbdb[j])
					{
						magicmovesbdb[j]=moves;
						magicmoves_b_indecies[i][index]=j;
						break;
					}
					else if(magicmovesbdb[j]==moves)
					{
						magicmoves_b_indecies[i][index]=j;
						break;
					}
				}
			#endif
		}
	}
	for(i=0;i<64;i++)
	{
		int squares[64];
		int numsquares=0;
		U64 temp=magicmoves_r_mask[i];
		while(temp)
		{
			U64 bit=temp&-temp;
			squares[numsquares++]=initmagicmoves_bitpos64_database[(bit*C64(0x07EDD5E59A4E28C2))>>58];
			temp^=bit;
		}
		for(temp=0;temp<(((U64)(1))<<numsquares);temp++)
		{
			U64 tempocc=initmagicmoves_occ(squares,numsquares,temp);
			#ifndef PERFECT_MAGIC_HASH
				RmagicNOMASK2(i,tempocc)=initmagicmoves_Rmoves(i,tempocc);
			#else
				U64 moves=initmagicmoves_Rmoves(i,tempocc);
				U64 index=(((tempocc)*magicmoves_r_magics[i])>>MINIMAL_R_BITS_SHIFT);
				int j;
				for(j=0;j<4900;j++)
				{
					if(!magicmovesrdb[j])
					{
						magicmovesrdb[j]=moves;
						magicmoves_r_indecies[i][index]=j;
						break;
					}
					else if(magicmovesrdb[j]==moves)
					{
						magicmoves_r_indecies[i][index]=j;
						break;
					}
				}
			#endif
		}
	}
}
