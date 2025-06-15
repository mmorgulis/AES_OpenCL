#include "utils/aes_core.h"
#include "utils/aes_host.h"
#include "utils/aes_mode.h"
#include "utils/aes_ctr.h"
#include "utils/aes_gcm.h"
#include "utils/safe_allocator.hpp"
#include <iomanip>

int main(int argc, char * argv[]) {
	// TODO MAIN WITH SOCKET WITH PRIVATE/PUBLIC KEYS + AES
	// Generate AES key and round keys
	crypto::safe_vector<uint8_t> aes_key_1 = generate_aes_key(256);

	AES_CTR ctr;
	ctr.set_key(aes_key_1);
	std::array<uint8_t, 12> iv_1 = {1,2,3,4,5,6,7,8,9,'a','b','c'};
	ctr.set_iv(iv_1);
	std::string pl = "Ciao mondo";
	auto cipher = ctr.encrypt(pl);
	auto plain = ctr.decrypt(cipher);
	std::cout << plain << std::endl;

	AES_GCM gcm;
	std::vector<uint8_t> aes_key = {
		0xfe, 0xff, 0xe9, 0x92,
		0x86, 0x65, 0x73, 0x1c,
		0x6d, 0x6a, 0x8f, 0x94,
		0x67, 0x30, 0x83, 0x08
	};

	std::vector<uint8_t> plain_text = {
		0xd9, 0x31, 0x32, 0x25, 0xf8, 0x84, 0x06, 0xe5,
		0xa5, 0x59, 0x09, 0xc5, 0xaf, 0xf5, 0x26, 0x9a,
		0x86, 0xa7, 0xa9, 0x53, 0x15, 0x34, 0xf7, 0xda,
		0x2e, 0x4c, 0x30, 0x3d, 0x8a, 0x31, 0x8a, 0x72,
		0x1c, 0x3c, 0x0c, 0x95, 0x95, 0x68, 0x09, 0x53,
		0x2f, 0xcf, 0x0e, 0x24, 0x49, 0xa6, 0xb5, 0x25,
		0xb1, 0x6a, 0xed, 0xf5, 0xaa, 0x0d, 0xe6, 0x57,
		0xba, 0x63, 0x7b, 0x39, 0x1a, 0xaf, 0xd2, 0x55
	};

	std::vector<uint8_t> cipher_text(plain_text.size());

	std::array<uint8_t, 12> iv = {
		0xca, 0xfe, 0xba, 0xbe,
		0xfa, 0xce, 0xdb, 0xad,
		0xde, 0xca, 0xf8, 0x88
	};

	gcm.set_key(aes_key);
	gcm.set_iv(iv);
	gcm.encrypt(plain_text, cipher_text);

	std::array<uint8_t, 16> tag = gcm.get_tag();
	std::cout << "PlainText: ";
	for (auto byte : plain_text) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << "\nCiphertext: ";
	for (auto byte : cipher_text) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << "\nTag: ";
	for (auto byte : tag) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << std::endl;

	AES_GCM gcm_dec;
	gcm_dec.set_key(aes_key);
	gcm_dec.set_iv(iv);
	gcm_dec.set_tag(tag);
	std::vector<uint8_t> pl_received(cipher_text.size());
	gcm_dec.decrypt(cipher_text, pl_received);
	std::cout << "PlainText Received: ";
	for (auto byte : pl_received) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}

	return 0;

}