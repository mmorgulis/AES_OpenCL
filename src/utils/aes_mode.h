#ifndef AES_MODE_H
#define AES_MODE_H

#include <span>
/**
* Interface to implement the concept of mode
*/
class AES_MODE {
public:
	virtual ~AES_MODE() = default;
	virtual void encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text) = 0;
	virtual void decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text) = 0;
	// Function that divide plain text in blocks and add padding
	virtual void elaborate_plain_text(std::span<const uint8_t> plain_text) = 0;
};

#endif // !AES_MODE_H