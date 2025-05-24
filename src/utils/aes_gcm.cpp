#include "aes_gcm.h"

void AES_GCM::set_key(std::span<const uint8_t> key) {
	if (key.size() != 16 && key.size() != 24 && key.size() != 32)
		_key.assign(key.begin(), key.end());
	else
		throw std::invalid_argument("Length error key");
}

void AES_GCM::set_iv(std::span<const uint8_t> iv) {
	if (iv.size() == 12)
		std::ranges::copy(iv, _iv.begin());
	else
		throw std::invalid_argument("Length error iv");
}

void AES_GCM::set_aad(std::span<const uint8_t> aad) {
	_aad.assign(aad.begin(), aad.end());
}

void AES_GCM::set_tag(std::span<const uint8_t> tag) {
	_tag.assign(tag.begin(), tag.end());
}

std::span<const uint8_t> AES_GCM::get_tag() const
{
	return std::span<const uint8_t>(_tag);
}

bool AES_GCM::control_attributes_encrypt() {
	// size controlled before
	return (AES_GCM::_key.size() >= 0 && AES_GCM::_iv.size() >= 0);
}
bool AES_GCM::control_attributes_decrypt() {
	return (control_attributes_encrypt() && _tag.size() != 0);
}

void AES_GCM::elaborate_plain_text(std::span<const uint8_t> plain_text) {
	// Maximum dimension consented by the counter (4 bytes at the end of IV)
	constexpr uint64_t max_plain_text_dim = uint64_t(1) << 32;
	if (plain_text.size() > max_plain_text_dim) {
		throw std::invalid_argument("Plain text too big");
	}

	size_t offset = 0;
	// Blocchi completi da 16 byte
	while (offset + 16 <= plain_text.size()) {
		std::span<const uint8_t> block = plain_text.subspan(offset, 16);
		std::array<uint8_t, 16> arr_block;
		std::copy(block.begin(), block.end(), arr_block.begin());
		_blocks.push_back(arr_block);
		offset += 16;
	}

	// Blocco finale (se parziale)
	if (offset < plain_text.size()) {
		std::span<const uint8_t> last_block = plain_text.subspan(offset);
		_last_block.insert(_last_block.end(), last_block.begin(), last_block.end());
	}
}


void AES_GCM::encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) {
	if (control_attributes_encrypt()) {
		elaborate_plain_text(plain_text);
		// Initialize counter
		std::array<uint8_t, 16> counter = {0};
		std::ranges::copy(_iv, counter.begin());
		counter[15] = 1;

	} else {
		throw std::invalid_argument("Argomenti non validi");
	}
}

// API semplice, prende stringa restituisce stringa
std::string AES_GCM::encrypt(std::string_view plain_text) {
	crypto::safe_vector<uint8_t> in(plain_text.begin(), plain_text.end());
    crypto::safe_vector<uint8_t> out(in.size());
    
    encrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
    return std::string(out.begin(), out.end());
}

void AES_GCM::decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text) {
	if (control_attributes_decrypt()) {

	} else {
		throw std::invalid_argument("Argomenti non validi");
	}
}

// API 
std::string AES_GCM::decrypt(std::string_view cipher_text) {
	crypto::safe_vector<uint8_t> in(cipher_text.begin(), cipher_text.end());
    crypto::safe_vector<uint8_t> out(in.size());
    
    decrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
    return std::string(out.begin(), out.end());
}
