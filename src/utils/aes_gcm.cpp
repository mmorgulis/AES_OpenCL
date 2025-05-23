#include "aes_gcm.h"

void AES_GCM::set_key(std::span<const uint8_t> key) {
	_key.assign(key.begin(), key.end());
}

void AES_GCM::set_iv(std::span<const uint8_t> iv) {
	_iv.assign(iv.begin(), iv.end());
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
	return ((AES_GCM::_key.size() == 16 || AES_GCM::_key.size() == 24 || AES_GCM::_key.size() == 32)
		&& AES_GCM::_iv.size() == 12);
}
bool AES_GCM::control_attributes_decrypt() {
	return (control_attributes_encrypt() && _tag.size() != 0);
}

void AES_GCM::elaborate_plain_text(std::span<const uint8_t> plain_text)
{
	// Scrivo in blocks i blocchi o uso variabile temporanea?
}

void AES_GCM::encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) {
	if (control_attributes_encrypt()) {
		elaborate_plain_text(plain_text);
	} else {
		throw std::invalid_argument("Argomenti non validi");
	}
}

void AES_GCM::decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text) {
	if (control_attributes_decrypt()) {

	} else {
		throw std::invalid_argument("Argomenti non validi");
	}
}
