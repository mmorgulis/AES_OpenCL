#ifndef GCM_AES_H
#define GCM_AES_H

#include <cstdint>
#include "aes_core.h"
#include "aes_mode.h"
#include "aes_host.h"
#include "safe_allocator.hpp"

class AES_GCM : public AES_MODE
{
private:
	crypto::safe_vector<std::array<uint8_t, 16>> _blocks; // 16 byte blocks
	crypto::safe_vector<uint8_t> _key;
	crypto::safe_vector<uint8_t> _round_keys;
	crypto::safe_vector<uint8_t> _iv;
	crypto::safe_vector<uint8_t> _tag;
	crypto::safe_vector<uint8_t> _aad;

public:
	AES_GCM() = default;
	~AES_GCM() = default;
	// Remove the possibility to copy the object mode
	AES_GCM& operator=(const AES_GCM& g) = delete;
	AES_GCM(const AES_GCM& g) = delete;
	
	// Functions to set attributes
	void set_key(std::span<const uint8_t> key);
	void set_iv(std::span<const uint8_t> key);
	void set_aad(std::span<const uint8_t> aad);
	void set_tag(std::span<const uint8_t> tag);
	std::span<const uint8_t> get_tag() const;

	bool control_attributes_encrypt();
	bool control_attributes_decrypt();

	void encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) override;
	void decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text) override;
	void elaborate_plain_text(std::span<const uint8_t> plain_text) override;
	

};


#endif // GCM_AES_H