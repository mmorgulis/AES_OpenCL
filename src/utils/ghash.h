#include "safe_allocator.hpp"
#include <array>

class GHASH {
private:
	crypto::safe_vector<uint8_t> _aad;
	std::array<uint8_t, 16> _tag;

	// Tables (byte level) for mult in GF(2^218)
	// 16 tables, with 256 values, each values of 16 bytes
	std::array<std::array<std::array<uint8_t, 16>, 256>, 16> m_table; // 64 KB
	std::array<uint8_t, 16> m_H;

public:
	void compute_H0();
};