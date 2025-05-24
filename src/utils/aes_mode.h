#ifndef AES_MODE_H
#define AES_MODE_H

#include <span>
#include <string_view>
/**
* Interface to implement the concept of mode
*/
class AES_MODE {
public:
	virtual ~AES_MODE() = default;
	virtual std::string encrypt(std::string_view plain_text) = 0;
	virtual std::string decrypt(std::string_view cipher_text) = 0;
	// Function that divide plain text in blocks and add padding
	virtual void elaborate_plain_text(std::span<const uint8_t> plain_text) = 0;
};

#endif // !AES_MODE_H