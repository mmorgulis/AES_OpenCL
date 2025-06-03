#include "aes_gcm.h"

void AES_GCM::set_key(std::span<const uint8_t> key) {
	ctr.set_key(key);
}

void AES_GCM::set_iv(std::span<const uint8_t> iv) {
	ctr.set_iv(iv);
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

std::string AES_GCM::encrypt(std::string_view plain_text) {
	return NULL;
}

std::string AES_GCM::decrypt(std::string_view cipher_text) {
	return NULL;
}

void AES_GCM::precompute_table(std::span<const uint8_t> counter)
{
}
