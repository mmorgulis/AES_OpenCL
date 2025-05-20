#ifndef GCM_AES_H
#define GCM_AES_H

#include <cstdint>
#include "aes_core.h"
#include "aes_host.h"
#include "safe_allocator.hpp"

class GCM_mode
{
private:
	crypto::safe_vector<uint8_t> round_keys;
	crypto::safe_vector<uint8_t> IV;

public:
	GCM_mode();
	~GCM_mode();

private:

};

GCM_mode::GCM_mode()
{
}

GCM_mode::~GCM_mode()
{
}

#endif // GCM_AES_H