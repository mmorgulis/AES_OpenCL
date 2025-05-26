#include "aes_gcm.h"
#include "aes_core.h"
#include "aes_host.h"
#include <algorithm>
#include <stdexcept>

AES_GCM::AES_GCM() {
	if (!find_platforms(platform, device))
		throw std::runtime_error("No platforms detected");
}

void AES_GCM::set_key(std::span<const uint8_t> key) {
	if (key.size() != 16 && key.size() != 24 && key.size() != 32)
		throw std::invalid_argument("Length error key");

	_key.assign(key.begin(), key.end());
	set_aes_parameters(key.size() * 8);

	// Key Schedule
	_round_keys = key_schedule(_key);
}

void AES_GCM::set_iv(std::span<const uint8_t> iv) {
	if (iv.size() != 12)
		throw std::invalid_argument("Length error tag");

	std::copy(iv.begin(), iv.end(), _iv.begin());
}
// Arbitrary length (can also be not setted)
// Padded to 16 bytes modules into GHASH function
void AES_GCM::set_aad(std::span<const uint8_t> aad) {
	_aad.assign(aad.begin(), aad.end());
}

void AES_GCM::set_tag(std::span<const uint8_t> tag) {
	if (tag.size() != 16)
		throw std::invalid_argument("Length error tag");

	std::copy(tag.begin(), tag.end(), _tag.begin());
}

std::span<const uint8_t> AES_GCM::get_tag() const
{
	return std::span<const uint8_t, 16>(_tag);
}

bool AES_GCM::control_attributes_encrypt() {
	// size controlled before
	return (AES_GCM::_key.size() >= 0 && AES_GCM::_iv.size() >= 0);
}
bool AES_GCM::control_attributes_decrypt() {
	return (control_attributes_encrypt() && _tag.size() != 0);
}

//void AES_GCM::elaborate_plain_text(std::span<const uint8_t> plain_text) {
//	// Maximum dimension consented by the counter (4 bytes at the end of IV)
//	constexpr uint64_t max_plain_text_dim = uint64_t(1) << 32;
//	if (plain_text.size() > max_plain_text_dim) {
//		throw std::invalid_argument("Plain text too big");
//	}
//
//	size_t offset = 0;
//	// Blocchi completi da 16 byte
//	while (offset + 16 <= plain_text.size()) {
//		std::span<const uint8_t> block = plain_text.subspan(offset, 16);
//		std::array<uint8_t, 16> arr_block;
//		std::copy(block.begin(), block.end(), arr_block.begin());
//		_blocks.push_back(arr_block);
//		offset += 16;
//	}
//
//	// Blocco finale (se parziale)
//	if (offset < plain_text.size()) {
//		std::span<const uint8_t> last_block = plain_text.subspan(offset);
//		_last_block.insert(_last_block.end(), last_block.begin(), last_block.end());
//	}
//}

void AES_GCM::precompute_table(std::span<const uint8_t> counter) {

}

static void inc_counter(std::span<uint8_t> counter) {
	// Increment by one the last for bytes of counter
	for (int i = 15; i < 12; --i) {
		if (++counter[i] != 0) break;
	}
}

void AES_GCM::encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) {
	if (!control_attributes_encrypt())
		throw std::invalid_argument("Invalid params");

	// Initialize counter
	std::array<uint8_t, 16> counter = { 0 };
	std::copy_n(_iv.begin(), 12, counter.begin());
	counter[15] = 1;

	// Compute H = AES_K(0)
	std::array<uint8_t, 16> H = { 0 };
	constexpr std::array<uint8_t, 16> zeros = { 0 };
	aes_encrypt(zeros, H, _round_keys);

	// Divide in blocks
	size_t full_blocks = plain_text.size() / 16;
	size_t remaining = plain_text.size() % 16;

	// Encrypt full blocks
	std::vector<uint8_t> counters(full_blocks * 16);
	for (size_t i = 0; i < full_blocks; ++i) {
		std::copy(counter.begin(), counter.end(), counters.begin() + i * 16);
		inc_counter(counter);
	}

	auto sub_cipher_text = cipher_text.first(full_blocks * 16);
	aes_encrypt(counters, sub_cipher_text, _round_keys);

	for (size_t i = 0; i < full_blocks * 16; ++i) {
		cipher_text[i] = plain_text[i] ^ sub_cipher_text[i];
	}

	// Last partial block
	if (remaining > 0) {
		std::array<uint8_t, 16> last_cipher_text;
		aes_encrypt(counter, last_cipher_text, _round_keys);

		size_t offset = full_blocks * 16;
		for (size_t i = 0; i < remaining; ++i) {
			cipher_text[offset + i] = plain_text[offset + i] ^ last_cipher_text[i];
		}
	}

	// GHASH will use H and plaintext/ciphertext + AAD
}


// API semplice, prende stringa restituisce stringa
std::string AES_GCM::encrypt(std::string_view plain_text) {
	crypto::safe_vector<uint8_t> in(plain_text.begin(), plain_text.end());
    crypto::safe_vector<uint8_t> out(in.size());
    
    encrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
    return std::string(out.begin(), out.end());
}

void AES_GCM::decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text) {
	if (!control_attributes_decrypt())
		throw std::invalid_argument("Invalid params");
	
	// Decrypt in AES is completely simmetrical of encrypt
	AES_GCM::encrypt(cipher_text, plain_text);
}

// API 
std::string AES_GCM::decrypt(std::string_view plain_text) {
	crypto::safe_vector<uint8_t> in(plain_text.begin(), plain_text.end());
    crypto::safe_vector<uint8_t> out(in.size());
    
    decrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
    return std::string(out.begin(), out.end());
}

void AES_GCM::clear() {
	_key.clear();
	_round_keys.clear();
	_iv.fill(0);
	_tag.fill(0);
	_aad.clear();
}
