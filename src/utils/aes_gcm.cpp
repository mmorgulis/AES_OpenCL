#include "aes_gcm.h"
#include <algorithm>

void AES_GCM::set_key(std::span<const uint8_t> key) {
	_ctr.set_key(key);
}

void AES_GCM::set_iv(std::span<const uint8_t> iv) {
	_ctr.set_iv(iv);
}

void AES_GCM::set_aad(std::span<const uint8_t> aad) {
	_ghash.set_aad(aad);
}

void AES_GCM::set_tag(std::span<const uint8_t> tag) {
	_ghash.set_tag(tag);
}

std::array<uint8_t, 16> AES_GCM::get_tag() const {
	return _ghash.get_tag();
}

void AES_GCM::compute_H0_Y0() {
	std::array<uint8_t, 16> counter_0 = { 0 };
	std::array<uint8_t, 16> H = { 0 };
	std::array<uint8_t, 16> Y = { 0 };

	// Compute H = AES_K(0^128)
	_ctr.encrypt_block(counter_0, H);
	_ghash.set_H(H);
	
	// Build IV || 0x00000001
	counter_0[15] = 0x01; // counter 1
	std::copy_n(_ctr.get_iv().begin(), 12, counter_0.begin());

	// Compute Y0 = AES_K(IV || 1)
	_ctr.encrypt_block(counter_0, Y); // Y0
	_ghash.set_EJ0(Y);

}

void AES_GCM::encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) {
	_ctr.encrypt(plain_text, cipher_text);
	compute_H0_Y0(); // only needed for tag
	_ghash.compute_ghash(cipher_text);
	_ghash.compute_tag(cipher_text);
}

void AES_GCM::encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text, std::span<const uint8_t> aad) {
	if(aad.size() != 0) _ghash.set_aad(aad);

	_ctr.encrypt(plain_text, cipher_text);
	compute_H0_Y0();
	_ghash.compute_ghash(cipher_text);
	_ghash.compute_tag(cipher_text);
}

void AES_GCM::encrypt_append_tag(const std::vector<uint8_t> &plain_text, std::vector<uint8_t> &cipher_text) {
	cipher_text.resize(plain_text.size());
	_ctr.encrypt(plain_text, cipher_text);
	compute_H0_Y0();
	_ghash.compute_ghash(cipher_text);
	_ghash.compute_tag(cipher_text);
	std::array<uint8_t, 16> tag = _ghash.get_tag();
	cipher_text.insert(cipher_text.end(), tag.begin(), tag.end());
}

std::string AES_GCM::encrypt(std::string_view plain_text) {
	// TAG and AAD already setted by caller
	crypto::safe_vector<uint8_t> in(plain_text.begin(), plain_text.end());
	crypto::safe_vector<uint8_t> out(in.size());

	encrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
	return std::string(out.begin(), out.end());
}

void AES_GCM::decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text) {
	if (!verify_tag(get_tag(), cipher_text)) {
		throw std::runtime_error("Error tag not concordant");
	}
	_ctr.decrypt(cipher_text, plain_text);

}

void AES_GCM::decrypt_tag_appended(const std::vector<uint8_t>& cipher_text_with_tag, std::vector<uint8_t>& plain_text) {
	std::array<uint8_t, 16> tag_appended = {0};
	std::copy_n(cipher_text_with_tag.end() - 16, 16, tag_appended.begin());
	set_tag(tag_appended);
	
	size_t ciphertext_size = cipher_text_with_tag.size() - 16;
	std::span<const uint8_t> cipher_text(cipher_text_with_tag.data(), ciphertext_size);

	plain_text.resize(ciphertext_size);
	decrypt(cipher_text, plain_text);
}


std::string AES_GCM::decrypt(const std::string_view cipher_text) {
	crypto::safe_vector<uint8_t> in(cipher_text.begin(), cipher_text.end());
	crypto::safe_vector<uint8_t> out(in.size());

	decrypt(std::span<const uint8_t>(in), std::span<uint8_t>(out));
	return std::string(out.begin(), out.end());
}

bool AES_GCM::verify_tag(const std::array<uint8_t, 16>& tag_received, std::span<const uint8_t> cipher_text) {
	_ghash.clear();
	compute_H0_Y0(); // write new _H and _Y
	_ghash.compute_ghash(cipher_text);
	_ghash.compute_tag(cipher_text);
	std::array<uint8_t, 16> tag_computed = get_tag();

	// Const time compare (no timing attack)
	uint8_t diff = 0;
	for (size_t i = 0; i < 16; ++i) {
		diff |= tag_received[i] ^ tag_computed[i];
	}

	return diff == 0;
}

void AES_GCM::clear() {
	_ghash.clear();
	_ctr.clear();
}

