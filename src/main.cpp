#include "utils/aes_core.h"
#include "utils/aes_host.h"
#include "utils/aes_mode.h"
#include "utils/aes_gcm.h"
#include "utils/cxxopts.hpp"
#include "utils/safe_allocator.hpp"

#include <filesystem>

#define CL_TARGET_OPENCL_VERSION 300
#define CL_HPP_TARGET_OPENCL_VERSION 300


int main(int argc, char * argv[]) {
	// TODO MAIN WITH SOCKET WITH PRIVATE/PUBLIC KEYS + AES
	// Generate AES key and round keys
	crypto::safe_vector<uint8_t> aes_key = generate_aes_key(256);
	crypto::safe_vector<uint8_t> round_keys = key_schedule(aes_key);

	AES_GCM gcm;
	gcm.set_key(aes_key);
	std::array<uint8_t, 12> iv = {1,2,3,4,5,6,7,8,9,'a','b','c'};
	gcm.set_iv(iv);
	std::string pl = "Ciao mondo, daje roma";
	auto cipher = gcm.encrypt(pl);
	auto plain = gcm.decrypt(cipher);
	std::cout << plain << std::endl;
	
	return 0;

}