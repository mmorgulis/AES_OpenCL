#include "ghash.h"
#include <algorithm>

// Arbitrary length (can also be not setted)
// Padded to 16 bytes modules into GHASH function
void GHASH::set_aad(std::span<const uint8_t> aad) {
	_aad.assign(aad.begin(), aad.end());
}

void GHASH::set_tag(std::span<const uint8_t> tag) {
	if (tag.size() != 16)
		throw std::invalid_argument("Length error tag");

	std::copy(tag.begin(), tag.end(), _tag.begin());
}

std::array<uint8_t, 16> GHASH::get_tag() const
{
	return _tag;
}

std::array<uint8_t, 16> GHASH::get_hash() const
{
	return _Y;
}

inline static std::array<uint8_t, 16> xor_array(const std::array<uint8_t, 16>& a, const std::array<uint8_t, 16>& b) {
	std::array<uint8_t, 16> x = { 0 };
	for (unsigned int i = 0; i < 16; ++i) {
		x[i] = a[i] ^ b[i];
	}
	return x;
}

void GHASH::update(std::span<const uint8_t> aad_bytes)
{
	std::array<uint8_t, 16> tmp = { 0 };
	for (size_t i = 0; i < aad_bytes.size(); i += 16) {
		std::array<uint8_t, 16> block = { 0 };
		size_t copy_bit = (aad_bytes.size() - i >= 16) ? 16 : aad_bytes.size() - i;
		std::copy_n(aad_bytes.begin() + i, copy_bit, block.begin());

		tmp = xor_array(_Y, block);
		_Y = multGF128_plain(tmp);
	}
}

/*
* Devo prendere tutti i blocchi del aad, ciphertext e lunghezze e
* avere come output un solo blocco da 16 byte. Per farlo processo
* in catena tutti i blocchi dell'aad con GMULT128 e ho in uscita
* un blocco e poi da questo lo faccio con il ciphertext e le lunghezze
*/
void GHASH::compute_ghash(std::span<const uint8_t> cipher_text)
{
	std::array<uint8_t, 16> tmp = { 0 };
	std::array<uint8_t, 16> tmp_Y = { 0 };

	// AAD
	for (size_t i = 0; i < _aad.size(); i += 16) {
		std::array<uint8_t, 16> block = { 0 };
		size_t copy_bit = (_aad.size() - i >= 16) ? 16 : _aad.size() - i;
		std::copy_n(_aad.begin() + i, copy_bit, block.begin());

		tmp = xor_array(tmp_Y, block);
		tmp_Y = multGF128_plain(tmp);
	}

	// CIPHERTEXT
	for (size_t i = 0; i < cipher_text.size(); i += 16) {
		std::array<uint8_t, 16> block = { 0 };
		size_t copy_bit = (cipher_text.size() - i >= 16) ? 16 : cipher_text.size() - i;
		std::copy_n(cipher_text.begin() + i, copy_bit, block.begin());

		tmp = xor_array(tmp_Y, block);
		tmp_Y = multGF128_plain(tmp);
	}

	// LENGTH A B
	std::array<uint8_t, 16> length_block = { 0 };
	uint64_t aad_bits = static_cast<uint64_t>(_aad.size()) * 8;
	uint64_t cip_bits = static_cast<uint64_t>(cipher_text.size()) * 8;

	for (int i = 0; i < 8; ++i) {
		length_block[i] = static_cast<uint8_t>((aad_bits >> (56 - 8 * i)) & 0xFF);
		length_block[8 + i] = static_cast<uint8_t>((cip_bits >> (56 - 8 * i)) & 0xFF);
	}

	tmp = xor_array(tmp_Y, length_block);
	tmp_Y = multGF128_plain(tmp);

	_Y = tmp_Y;

}

void GHASH::compute_tag(std::span<const uint8_t> cipher_text)
{
	compute_ghash(cipher_text);
	_tag = xor_array(_Y, _EJ0);
}

inline static std::array<uint8_t, 16> mult2_and_reduce(const std::array<uint8_t, 16>& row) {
	std::array<uint8_t, 16> result = { 0 };
	// Mult by 2 in GF(2) is equal to a shift
	bool carry = (row[0] & 0b10000000) != 0; // Control if overflow
	for (unsigned int i = 0; i < 16; ++i) {
		uint8_t next = (i < 15) ? row[i + 1] : 0;
		result[i] = (row[i] << 1) | (next >> 7); // shift by 1 for all bytes and the MSB in last place
	}

	if (carry) {
		result[15] ^= 0x87; // 0x87 is x^128 mod P = x^7 + x^2 + x + 1
	}
	return result;
	
}

std::array<uint8_t, 16> GHASH::multGF128_plain(const std::array<uint8_t, 16> &X) const {
	std::array<uint8_t, 16> Z = {0};        
	std::array<uint8_t, 16> V = _H;          

	for (int i = 0; i < 128; ++i) {
		int byte_pos = i / 8;
		int bit_pos = 7 - (i % 8);

		if ((X[byte_pos] >> bit_pos) & 1) {
			Z = xor_array(Z, V);
		}

		bool lsb = V[15] & 1;
		for (int j = 15; j > 0; --j) {
			V[j] = (V[j] >> 1) | ((V[j - 1] & 1) << 7);
		}
		V[0] >>= 1;
		if (lsb) {
			V[0] ^= 0xe1; // x^7 + x^2 + x + 1
		}
	}

	return Z;
}

void GHASH::set_H(const std::array<uint8_t, 16> &H)
{
	std::copy(H.begin(), H.end(), _H.begin());

	//precompute_tables(); //set tables
}

void GHASH::set_EJ0(const std::array<uint8_t, 16>& J)
{
	std::copy(J.begin(), J.end(), _EJ0.begin());
}

void GHASH::clear() {
	_aad.clear();
	_tag.fill(0);
	_H.fill(0);
	_EJ0.fill(0);
	/*for (auto& table : _m_table) {
		for (auto& row : table) {
			row.fill(0);
		}
	}*/
}
