#include "chessboard.h"
#include "util.h"
using namespace std;


#define B64(a) ((quint64)1 << (a))	/* returns a 64-bit bitmask with <a> bit set */

const Castling castling = {
	{ {{Chessboard::E1, Chessboard::G1}, {Chessboard::E1, Chessboard::C1}}, {{Chessboard::E8, Chessboard::G8}, {Chessboard::E8, Chessboard::C8}} },
	{ {{Chessboard::H1, Chessboard::F1}, {Chessboard::A1, Chessboard::D1}}, {{Chessboard::H8, Chessboard::F8}, {Chessboard::A8, Chessboard::D8}} },
	{ {BIT(0U), BIT(1U)}, {BIT(2U), BIT(3U)} },
	{ BIT(0U) | BIT(1U), BIT(2U) | BIT(3U) }
};

static int randomSeed = 1;	/* seed for the random number generator */

/* An array of bitmasks where each mask has one bit set.  */
const quint64 bit64[64] =
{
	B64(0),  B64(1),  B64(2),  B64(3),  B64(4),  B64(5),  B64(6),  B64(7),
	B64(8),  B64(9),  B64(10), B64(11), B64(12), B64(13), B64(14), B64(15),
	B64(16), B64(17), B64(18), B64(19), B64(20), B64(21), B64(22), B64(23),
	B64(24), B64(25), B64(26), B64(27), B64(28), B64(29), B64(30), B64(31),
	B64(32), B64(33), B64(34), B64(35), B64(36), B64(37), B64(38), B64(39),
	B64(40), B64(41), B64(42), B64(43), B64(44), B64(45), B64(46), B64(47),
	B64(48), B64(49), B64(50), B64(51), B64(52), B64(53), B64(54), B64(55),
	B64(56), B64(57), B64(58), B64(59), B64(60), B64(61), B64(62), B64(63)
};

/* Returns true if <square> is a valid square.  */
bool
isOnBoard(int square)
{
	if (square >= 0 && square <= 63)
		return true;
	return false;
}

/* The "minimal standard" random number generator by Park and Miller.
   Returns a pseudo-random integer between 1 and 2147483646.  */
int
myRand(void)
{
	const int a = 16807;
	const int m = 2147483647;
	const int q = (m / a);
	const int r = (m % a);

	int hi = randomSeed / q;
	int lo = randomSeed % q;
	int test = a * lo - r * hi;

	if (test > 0)
		randomSeed = test;
	else
		randomSeed = test + m;

	return randomSeed;
}

/* Initialize the random number generator with a new seed.  */
void
setRandSeed(int new_seed)
{
	randomSeed = new_seed;
}


#if defined(__GNUC__) && (defined(__LP64__) || defined(__powerpc64__))

/* Locates the first (least significant) "one" bit in a bitboard.
   Optimized for x86-64.  */
int
getLsb(quint64 b)
{
	quint64 ret;
	__asm__
	(
        "bsfq %[b], %[ret]"
        :[ret] "=r" (ret)
        :[b] "mr" (b)
	);
	return (int)ret;
}

#else /* not 64-bit GNUC */

/* Locates the first (least significant) "one" bit in a bitboard.
   Optimized for x86.  */
int
getLsb(quint64 b)
{
	unsigned a;
	/* Based on code by Lasse Hansen.  */
	static const int lsb_table[32] = {
		31,  0,  9,  1, 10, 20, 13,  2,
		 7, 11, 21, 23, 17, 14,  3, 25,
		30,  8, 19, 12,  6, 22, 16, 24,
		29, 18,  5, 15, 28,  4, 27, 26
	};

	Q_ASSERT(b != 0);

	a = (unsigned)b;
	if (a != 0)
		return lsb_table[((a & -(int)a) * 0xe89b2be) >> 27];
	a  =  (unsigned)(b >> 32);

	return lsb_table[((a & -(int)a) * 0xe89b2be) >> 27]  +  32;
}

#endif /* not 64-bit GNUC */


#if defined(__LP64__) || defined(__powerpc64__) || defined(_WIN64)

/* Returns the number of "one" bits in a 64-bit word.
   Optimized for x86-64.  */
int
popcount(quint64 b)
{
	b = (b & 0x5555555555555555) + ((b >> 1) & 0x5555555555555555);
	b = (b & 0x3333333333333333) + ((b >> 2) & 0x3333333333333333);
	b = (b + (b >> 4)) & 0x0F0F0F0F0F0F0F0F;
	b = b + (b >> 8);
	b = b + (b >> 16);
	b = (b + (b >> 32)) & 0x0000007F;

	return (int)b;
}
#else /* not 64-bit */

/* From R. Scharnagl

   Returns the number of "one" bits in a 64-bit word.
   Endian independent form.
   Optimized for 32-bit processors.  */
int
popcount(quint64 b)
{
	unsigned buf;
	unsigned acc;

	if (b == 0)
		return 0;

	buf = (unsigned)b;
	acc = buf;
	acc -= ((buf &= 0xEEEEEEEE) >> 1);
	acc -= ((buf &= 0xCCCCCCCC) >> 2);
	acc -= ((buf &= 0x88888888) >> 3);
	buf = (unsigned)(b >> 32);
	acc += buf;
	acc -= ((buf &= 0xEEEEEEEE) >> 1);
	acc -= ((buf &= 0xCCCCCCCC) >> 2);
	acc -= ((buf &= 0x88888888) >> 3);
	acc = (acc & 0x0F0F0F0F) + ((acc >> 4) & 0x0F0F0F0F);
	acc = (acc & 0xFFFF) + (acc >> 16);

	return (acc & 0xFF) + (acc >> 8);
}
#endif /* not 64-bit */

/* Same as getLsb(), but also clears the first bit in *b.  */
int
popLsb(quint64 *b)
{
	int lsb;

	Q_ASSERT(b != NULL);
	
	lsb = getLsb(*b);
	*b &= (*b - 1);

	return lsb;
}

