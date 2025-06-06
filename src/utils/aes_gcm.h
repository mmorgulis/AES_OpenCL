#ifndef AES_GCM_H
#define AES_GCM_H

#include "aes_mode.h"
#include "aes_ctr.h"
#include "ghash.h"
#include <memory>
#include <span>

class AES_GCM : public AES_MODE {
private:
	AES_CTR ctr; // Composition
	GHASH ghash;
public:
	std::array<uint8_t, 16> compute_H0();

public:
	AES_GCM() = default;
	~AES_GCM() = default;
	// Remove the possibility to copy the object mode
	AES_GCM& operator=(const AES_GCM& g) = delete;
	AES_GCM(const AES_GCM& g) = delete;

	// Functions to set attributes
	void set_key(std::span<const uint8_t> key);
	void set_iv(std::span<const uint8_t> iv);
	void set_aad(std::span<const uint8_t> aad);
	std::array<uint8_t, 16> get_tag() const;

	std::string encrypt(std::string_view plain_text) override;
	void encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text);
	std::string decrypt(std::string_view cipher_text) override;
	void decrypt(std::span<uint8_t> cipher_text, std::span<const uint8_t> plain_text);

	void clear();

};



#endif // !AES_GCM_H