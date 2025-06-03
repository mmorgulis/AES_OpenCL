#ifndef AES_GCM_H
#define AES_GCM_H

#include "aes_mode.h"
#include "aes_ctr.h"
#include <memory>
#include <span>

class AES_GCM : public AES_MODE {
private:
	AES_CTR ctr; // Composition
	crypto::safe_vector<uint8_t> _aad;
	std::array<uint8_t, 16> _tag;

	// Tables (byte level) for mult in GF(2^218)
	// 16 tables, with 256 values, each values of 16 bytes
	std::array<std::array<std::array<uint8_t, 16>, 256>, 16> m_table; // 64 KB
	std::array<uint8_t, 16> m_H;

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
	void set_tag(std::span<const uint8_t> tag);
	std::span<const uint8_t> get_tag() const;

	std::string encrypt(std::string_view plain_text) override;
	std::string decrypt(std::string_view cipher_text) override;

	void precompute_table(std::span<const uint8_t> counter);

	void clear();

};



#endif // !AES_GCM_H