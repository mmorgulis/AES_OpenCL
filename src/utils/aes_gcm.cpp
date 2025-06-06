#include "aes_gcm.h"
#include <algorithm>

void AES_GCM::set_key(std::span<const uint8_t> key) {
	ctr.set_key(key);
}

void AES_GCM::set_iv(std::span<const uint8_t> iv) {
	ctr.set_iv(iv);
}

void AES_GCM::set_aad(std::span<const uint8_t> aad) {
	ghash.set_aad(aad);
}

std::array<uint8_t, 16> AES_GCM::get_tag() const {
	return ghash.get_tag();
}

std::array<uint8_t, 16> AES_GCM::compute_H0() {
	std::array<uint8_t, 16> counter_0 = { 0 };
	std::array<uint8_t, 16> H = { 0 };
	std::array<uint8_t, 16> Y = { 0 };
	std::copy_n(ctr.get_iv().begin(), 12, counter_0.begin());
	ctr.encrypt_block(counter_0, H);
	ghash.set_H(H);
	
	counter_0[15] = 0x01; // counter 1
	ctr.encrypt_block(counter_0, Y); // Y0
	ghash.set_EJ0(Y);

	return H;

}

void AES_GCM::encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) {
	
}

void AES_GCM::decrypt(std::span<uint8_t> cipher_text, std::span<const uint8_t> plain_text) {

}

std::string AES_GCM::encrypt(std::string_view plain_text) {
	return NULL;
}

std::string AES_GCM::decrypt(std::string_view cipher_text) {
	return NULL;
}

void AES_GCM::clear() {
	ghash.clear();
	ctr.clear();
}

