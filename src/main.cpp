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
	std::vector<uint8_t> plaintext = {
		0x53, 0x69, 0x6e, 0x67, 0x6c, 0x65, 0x20, 0x62,
		0x6c, 0x6f, 0x63, 0x6b, 0x20, 0x6d, 0x73, 0x67,

		0x20, 0x66, 0x6f, 0x72, 0x20, 0x41, 0x45, 0x53,
		0x2d, 0x43, 0x54, 0x52, 0x20, 0x6d, 0x6f, 0x64
	};

	std::vector<uint8_t> ciphertext(plaintext.size());

	std::vector<uint8_t> expected_ciphertext = {
		0xe4, 0x09, 0x5d, 0xff, 0x0b, 0x81, 0x1c, 0x52,
		0xcf, 0xf8, 0x0e, 0x1d, 0xe2, 0x6b, 0xb8, 0x5a,

		0xd0, 0x36, 0xce, 0xf0, 0x78, 0x2c, 0xe2, 0x4c,
		0x8a, 0x84, 0xda, 0x4f, 0x6a, 0x26, 0x8f, 0xea
	};

	AES_CTR ctr;
	ctr.set_key(aes_key);
	ctr.set_iv(iv);
	ctr.encrypt(plaintext, ciphertext);
	for (size_t i = 0; i < ciphertext.size(); ++i) {
		std::cout << std::hex << std::setw(2) << std::setfill('0')
			<< static_cast<int>(ciphertext[i]) << " ";
		if ((i + 1) % 16 == 0) std::cout << "\n";
	}
	ctr.decrypt(ciphertext, plaintext);
	

	return 0;

}