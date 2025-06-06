#ifndef CTR_AES_H
#define CTR_AES_H

#include "aes_mode.h"
#include "aes_host.h"
#include "safe_allocator.hpp"
#include <cstdint>
#include <string>
#include <array>
#include <span>

class AES_CTR : public AES_MODE
{
private:
	crypto::safe_vector<uint8_t> _key;
	crypto::safe_vector<uint8_t> _round_keys;
	std::array<uint8_t, 12> _iv{}; // more efficient dimension for iv
	CTROpenCL _accelerator;
	
public:
	AES_CTR() = default;
	~AES_CTR() = default;
	// Remove the possibility to copy the object mode
	AES_CTR& operator=(const AES_CTR& g) = delete;
	AES_CTR(const AES_CTR& g) = delete;
	
	// Functions to set attributes
	void set_key(std::span<const uint8_t> key);
	void set_iv(std::span<const uint8_t> iv);
	std::array<uint8_t, 12> get_iv() const;
	
	std::span<const uint8_t> get_tag() const;

	bool control_attributes();

	std::string encrypt(std::string_view plain_text) override;
	std::string decrypt(std::string_view cipher_text) override;
	void encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text);
	void decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text);

	void encrypt_block(std::array<uint8_t, 16>& plain_text, std::array<uint8_t, 16>& cipher_text);

	void clear();


	
};


#endif // CTR_AES_H