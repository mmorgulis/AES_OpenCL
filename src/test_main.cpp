#include "utils/aes_core.h"
#include "utils/aes_host.h"
#include "utils/cxxopts.hpp"
#include "utils/safe_allocator.hpp"

#include <filesystem>


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
		break;
	case 192: 
		aes_version_define = "-DAES_192"; 
		num_round_keys = 13; 
		break;
	case 256:
		// Already defined variables in aes_core.h
		break;
	default: 
		throw std::runtime_error("Unsupported Key Length"); 
	}
	crypto::safe_vector<uint8_t> plain_text = {
		0x6b, 0xc1, 0xbe, 0xe2,
		0x2e, 0x40, 0x9f, 0x96,
		0xe9, 0x3d, 0x7e, 0x11,
		0x73, 0x93, 0x17, 0x2a
	};
	
	crypto::safe_vector<uint8_t> cipher_text;
	cipher_text.resize(plain_text.size());

	// The safe allocator of type T is used automatically
	// Alloc 16 byte by default (empty vector dimension)
	crypto::safe_vector<uint8_t> aes_key(key_length/8);
	
	// Bytes for round_keys array
	unsigned int round_keys_size = num_round_keys * key_length/8; // bytes
	crypto::safe_vector<uint8_t> round_keys;

	// Generate AES keys
	generate_aes_key(aes_key, round_keys);

	/*
	// Print key
	for (uint8_t byte : aes_key) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << '\n'; 

	// Print round keys
	int i = 0;
	for (auto key_byte : round_keys) {
		if (i != 0 && i % 4 == 0)
			std::cout << std::endl;
		
		std::cout << std::hex << static_cast<int>(key_byte) << " ";
		++i;
	}
	std::cout << std::endl;
	*/

	if (!find_platforms(platform, device)) {
		throw std::runtime_error("No platforms detected");
	}
	
	/*
	// Print device
	std::cout << "Platform chosen: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	std::cout << "Device chosen: " << device.getInfo<CL_DEVICE_NAME>() << "\n";
	*/

	return 0;

}