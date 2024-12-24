#pragma once
#include <cstdint>

#if defined(_M_AMD64)
#include <intrin.h>
#endif

#if defined(__GNUC__)

FORCEINLINE int PopCnt(uint32_t bb) {
	return __builtin_popcount(bb);
}
FORCEINLINE int PopCnt(uint64_t bb) {
	return __builtin_popcountll(bb);
}
FORCEINLINE int PopCnt15(uint64_t bb) {
	return __builtin_popcountll(bb);
}
FORCEINLINE int PopCntSparse(uint64_t bb) {
	return __builtin_popcountll(bb);
}

#elif defined(_M_AMD64) 

inline uint8_t PopCnt(uint32_t bb) {
	return (uint8_t)_mm_popcnt_u32(bb);
}
inline uint8_t PopCnt(uint64_t bb) {
	return (uint8_t)_mm_popcnt_u64(bb);
}
inline uint8_t PopCnt15(uint64_t bb) {
	return (uint8_t)_mm_popcnt_u64(bb);
}
inline uint8_t PopCntSparse(uint64_t bb) {
	return (uint8_t)_mm_popcnt_u64(bb);
}
 
#else

// general purpose population count
template<typename T>
constexpr int PopCnt(T bb)
{
	STATIC_ASSERT(std::is_integral<T>::value && std::is_unsigned<T>::value);
	return std::bitset<sizeof(T) * 8>(bb).count();
}
template<>
inline int PopCnt(uint64_t bb) {
	const uint64_t k1 = (uint64_t)0x5555555555555555;
	const uint64_t k2 = (uint64_t)0x3333333333333333;
	const uint64_t k3 = (uint64_t)0x0F0F0F0F0F0F0F0F;
	const uint64_t k4 = (uint64_t)0x0101010101010101;
	bb -= (bb >> 1) & k1;
	bb = (bb & k2) + ((bb >> 2) & k2);
	bb = (bb + (bb >> 4)) & k3;
	return (bb * k4) >> 56;
}
// faster version assuming not more than 15 bits set, used in mobility
// eval, posted on CCC forum by Marco Costalba of Stockfish team
inline int PopCnt15(uint64_t bb) {
	unsigned w = unsigned(bb >> 32), v = unsigned(bb);
	v -= (v >> 1) & 0x55555555; // 0-2 in 2 bits
	w -= (w >> 1) & 0x55555555;
	v = ((v >> 2) & 0x33333333) + (v & 0x33333333); // 0-4 in 4 bits
	w = ((w >> 2) & 0x33333333) + (w & 0x33333333);
	v += w; // 0-8 in 4 bits
	v *= 0x11111111;
	return int(v >> 28);
}
// version faster on sparsely populated bitboards
inline int PopCntSparse(uint64_t bb) {
	int count = 0;
	while (bb) {
		count++;
		bb &= bb - 1;
	}
	return count;
}

#endif
/*----------------------------------------------------------------*/