#ifndef GHASH_H
#define GHASH_H

#include "safe_allocator.hpp"
#include <array>
#include <span>

class GHASH {
private:
	std::vector<uint8_t> _aad;
	std::array<uint8_t, 16> _tag{};

	// Tables (byte level) for mult in GF(2^218)
	// 16 tables, with 256 values, each values of 16 bytes
	//std::array<std::array<std::array<uint8_t, 16>, 256>, 16> _m_table{}; // 64 KB
	std::array<uint8_t, 16> _H{}; // ghash key = 16 bytes 
	std::array<uint8_t, 16> _EJ0{}; // counter1 encrypted with aes
	std::array<uint8_t, 16> _Y{}; // GHASH 

public:
	GHASH() = default;
	~GHASH() = default;
	void set_H(const std::array<uint8_t, 16>& H);
	void set_EJ0(const std::array<uint8_t, 16>& J);
	void clear();
	void set_aad(std::span<const uint8_t> aad);
	void set_tag(std::span<const uint8_t> tag);
	std::array<uint8_t, 16> get_tag() const;
	std::array<uint8_t, 16> get_hash() const;

	void update(std::span<const uint8_t> aad_block);
	void compute_ghash(std::span<const uint8_t> cipher_text);
	void compute_tag(std::span<const uint8_t> cipher_text);
	/*void precompute_tables();
	std::array<uint8_t, 16> multGF128(std::array<uint8_t, 16> x);*/
	std::array<uint8_t, 16> multGF128_plain(const std::array<uint8_t, 16> &X) const;
};

#endif // !GHASH_H