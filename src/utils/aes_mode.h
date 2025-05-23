#include "safe_allocator.hpp"
/**
* Abstract class to implement the concept of mode
*/
class AES_MODE {
	private:
		crypto::safe_vector<uint8_t> _key;
		crypto::safe_vector<uint8_t> _iv;

	public:
		virtual ~AES_MODE() = default;
		virtual void encrypt() = 0;
		virtual void decrypt() = 0;
		// Function that divide plain text in blocks and add padding
		virtual void elaborate_plain_text(crypto::safe_vector<uint8_t> plain_text) = 0;
};