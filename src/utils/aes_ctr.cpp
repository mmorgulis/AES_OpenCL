#include "aes_ctr.h"
#include "aes_core.h"
#include "aes_host.h"
#include <algorithm>
#include <stdexcept>

AES_CTR::~AES_CTR() {
	clear();
}

void AES_CTR::set_key(std::span<const uint8_t> key) {
	if (key.size() != 16 && key.size() != 24 && key.size() != 32)
		throw std::invalid_argument("Length error key");

	_key.assign(key.begin(), key.end());
	set_aes_parameters((unsigned int) key.size() * 8);

	// Key Schedule
	_round_keys = key_schedule(_key);
}

void AES_CTR::set_iv(std::span<const uint8_t> iv) {
	if (iv.size() != 12)
		throw std::invalid_argument("Length error tag");

	std::copy(iv.begin(), iv.end(), _iv.begin());
}

bool AES_CTR::control_attributes() {
	// size controlled before
	return (AES_CTR::_key.size() >= 0 && AES_CTR::_iv.size() >= 0);
}

void AES_CTR::encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) {
	if (!control_attributes())
		throw std::invalid_argument("Invalid params");

	_accelerator.aes_ctr_encrypt(plain_text, cipher_text, _round_keys, _iv);
}


// API semplice, prende stringa restituisce stringa
std::string AES_CTR::encrypt(std::string_view plain_text) {
	crypto::safe_vector<uint8_t> in(plain_text.begin(), plain_text.end());
    crypto::safe_vector<uint8_t> out(in.size());
    
    encrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
    return std::string(out.begin(), out.end());
}

void AES_CTR::decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text) {
	if (!control_attributes())
		throw std::invalid_argument("Invalid params");
	
	// Decrypt in AES is completely simmetrical of encrypt
	AES_CTR::encrypt(cipher_text, plain_text);
}

// API 
std::string AES_CTR::decrypt(std::string_view plain_text) {
	crypto::safe_vector<uint8_t> in(plain_text.begin(), plain_text.end());
    crypto::safe_vector<uint8_t> out(in.size());
    
    decrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
    return std::string(out.begin(), out.end());
}

void AES_CTR::clear() {
	_key.clear();
	_round_keys.clear();
	_iv.fill(0);
}
