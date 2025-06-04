#include "utils/aes_core.h"
#include "utils/aes_host.h"
#include "utils/aes_mode.h"
#include "utils/aes_gcm.h"
#include "utils/safe_allocator.hpp"
#include <iomanip>

int main(int argc, char * argv[]) {
	// TODO MAIN WITH SOCKET WITH PRIVATE/PUBLIC KEYS + AES
	// Generate AES key and round keys
	/*crypto::safe_vector<uint8_t> aes_key = generate_aes_key(256);
	crypto::safe_vector<uint8_t> round_keys = key_schedule(aes_key);

	AES_CTR ctr;
	ctr.set_key(aes_key);
	std::array<uint8_t, 12> iv = {1,2,3,4,5,6,7,8,9,'a','b','c'};
	ctr.set_iv(iv);
	std::string pl = "Ciao mondo, daje roma";
	auto cipher = ctr.encrypt(pl);
	auto plain = ctr.decrypt(cipher);
	std::cout << plain << std::endl;*/
	
	crypto::safe_vector<uint8_t> aes_key = {
	0xae, 0x68, 0x52, 0xf8, 0x12, 0x10, 0x67, 0xcc,
	0x4b, 0xf7, 0xa5, 0x76, 0x55, 0x77, 0xf3, 0x9e
	};

	std::vector<uint8_t> iv = {
	0x00, 0x00, 0x00, 0x30,  // nonce (4 byte)
	0x00, 0x00, 0x00, 0x00,  // part of IV
	0x00, 0x00, 0x00, 0x00
	};

	// Plaintext
	std::string plaintext = "Ciao Mondo!!";

	AES_CTR ctr;
	ctr.set_key(aes_key);
	ctr.set_iv(iv);
	std::string ciphertext = ctr.encrypt(plaintext);
	std::string new_pl = ctr.decrypt(ciphertext);
	std::cout << new_pl << std::endl;

	return 0;

}