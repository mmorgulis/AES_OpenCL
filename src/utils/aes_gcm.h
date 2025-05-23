#ifndef GCM_AES_H
#define GCM_AES_H

#include <cstdint>
#include "aes_core.h"
#include "aes_mode.h"
#include "aes_host.h"
#include "safe_allocator.hpp"

class AES_GCM : AES_MODE
{
private:
	crypto::safe_vector<std::array<uint8_t, 16>> _blocks; // 16 byte blocks
	crypto::safe_vector<uint8_t> _tag;
	crypto::safe_vector<uint8_t> _aad;

public:
	// Remove possibility to creare empty object
	AES_GCM() = delete;
	~AES_GCM() = default;

	// Constructor to use
	AES_GCM(crypto::safe_vector<uint8_t> round_keys);

	// Remove the possibility to copy the object mode
	AES_GCM& operator=(const AES_GCM& g) = delete;
	AES_GCM(const AES_GCM& g) = delete;
	




};


#endif // GCM_AES_H