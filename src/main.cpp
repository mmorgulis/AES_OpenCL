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
	
	/*
	* TODO : modifico per prendere un plaintext opzionale 
	* da riga di comando in forma normale
	*/

	/*
	// Take the key length as input argument
	cxxopts::Options options("OpenCL_cryptography", "AES implementation");
	options.add_options()
		("k,key-length", "Key length", cxxopts::value<int>());
	auto result = options.parse(argc, argv);
	key_length = result["key-length"].as<int>(); */
	
	// Variables bases on key length
	switch (key_length) {
	case 128:
		aes_version_define = "-DAES_128";
		num_round_keys = 11;
		num_rounds = 10;
		break;
	case 192:
		aes_version_define = "-DAES_192";
		num_round_keys = 13;
		num_rounds = 12;
		break;
	case 256:
		aes_version_define = "-DAES_256";
		num_round_keys = 15;
		num_rounds = 14;
		break;
	default: 
		throw std::runtime_error("Unsupported Key Length"); 
	}

	crypto::safe_vector<uint8_t> plain_text = {
		0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a
	};
	
	crypto::safe_vector<uint8_t> cipher_text(plain_text.size());
	
	// Generate AES key and round keys
	crypto::safe_vector<uint8_t> aes_key = generate_aes_key();
	crypto::safe_vector<uint8_t> round_keys = key_schedule(aes_key);
	/*
	// Print round keys
	int i = 0;
	for (auto key_byte : round_keys) {
		if (i != 0 && i % 4 == 0)
			std::cout << std::endl;
		
		std::cout << std::hex << static_cast<int>(key_byte) << " ";
		++i;
	}
	std::cout << std::endl; */

	if (!find_platforms(platform, device)) {
		throw std::runtime_error("No platforms detected");
	}
	/*
	// Print device
	std::cout << "Platform chosen: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	std::cout << "Device chosen: " << device.getInfo<CL_DEVICE_NAME>() << "\n";
	*/

	aes_encrypt(plain_text, cipher_text, round_keys);
	std::cout << "CipherText: ";
	for (auto byte : cipher_text) {
		std::cout << std::hex << static_cast<int>(byte) << " "; 
	}
	
	//crypto::safe_vector<uint8_t> inv_round_keys = invert_round_keys(round_keys);
	crypto::safe_vector<uint8_t> recived_plaintext;
	recived_plaintext.resize(cipher_text.size());
	aes_decrypt(cipher_text, recived_plaintext, round_keys);
	std::cout << "\nPlainText: ";
	for (auto byte : plain_text) {
		std::cout << std::hex << static_cast<int>(byte) << " "; 
	}
	std::cout << "\nPlainText recived: ";
	for (auto byte : recived_plaintext) {
		std::cout << std::hex << static_cast<int>(byte) << " "; 
	}

	AES_GCM gcm;
	gcm.set_key(aes_key);
	std::array<uint8_t, 12> iv = {1,2,3,4,5,6,7,8,9,'a','b','c'};
	gcm.set_iv(iv);
	std::string pl = "Ciao mondo, daje roma";
	auto cipher = gcm.encrypt(pl);
	auto plain = gcm.decrypt(cipher);
	std::cout << '\n' << plain << std::endl;
	
	return 0;

}