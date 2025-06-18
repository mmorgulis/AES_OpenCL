/*
* GCM specification official test
* Test Case 2

* Test Case 3 
	K			feffe9928665731c6d6a8f9467308308 
	P			d9313225f88406e5a55909c5aff5269a 
				86a7a9531534f7da2e4c303d8a318a72 
				1c3c0c95956809532fcf0e2449a6b525 
				b16aedf5aa0de657ba637b391aafd255 
	IV			cafebabefacedbaddecaf888 
	H			b83b533708bf535d0aa6e52980d53b78 
	Y0			cafebabefacedbaddecaf88800000001 
	E(K, Y0)	3247184b3c4f69a44dbcd22887bbb418 
	Y1			cafebabefacedbaddecaf88800000002 
	E(K, Y1)	9bb22ce7d9f372c1ee2b28722b25f206 
	Y2			cafebabefacedbaddecaf88800000003 
	E(K, Y2)	650d887c3936533a1b8d4e1ea39d2b5c 
	Y3			cafebabefacedbaddecaf88800000004 
	E(K, Y3)	3de91827c10e9a4f5240647ee5221f20 
	Y4			cafebabefacedbaddecaf88800000005 
	E(K, Y4)	aac9e6ccc0074ac0873b9ba85d908bd0 
	X1			59ed3f2bb1a0aaa07c9f56c6a504647b 
	X2			b714c9048389afd9f9bc5c1d4378e052 
	X3			47400c6577b1ee8d8f40b2721e86ff10 
	X4			4796cf49464704b5dd91f159bb1b7f95 
	C			42831ec2217774244b7221b784d0d49c 
				e3aa212f2c02a4e035c17e2329aca12e 
				21d514b25466931c7d8f6a5aac84aa05 
				1ba30b396a0aac973d58e091473f5985 
	T			4d5c2af327cd64a62cf35abd2ba6fab4 
*/

#include "utils/aes_gcm.h"
#include <iomanip>

int main(int argc, char* argv[]) {

	bool test_failed = 0;

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

	std::vector<uint8_t> exp_cipher_text = {
		0x42, 0x83, 0x1e, 0xc2, 0x21, 0x77, 0x74, 0x24, 0x4b, 0x72, 0x21, 0xb7, 0x84, 0xd0, 0xd4, 0x9c,
		0xe3, 0xaa, 0x21, 0x2f, 0x2c, 0x02, 0xa4, 0xe0, 0x35, 0xc1, 0x7e, 0x23, 0x29, 0xac, 0xa1, 0x2e,
		0x21, 0xd5, 0x14, 0xb2, 0x54, 0x66, 0x93, 0x1c, 0x7d, 0x8f, 0x6a, 0x5a, 0xac, 0x84, 0xaa, 0x05,
		0x1b, 0xa3, 0x0b, 0x39, 0x6a, 0x0a, 0xac, 0x97, 0x3d, 0x58, 0xe0, 0x91, 0x47, 0x3f, 0x59, 0x85
	};

	std::array<uint8_t, 16> exp_tag = {
		0x4d, 0x5c, 0x2a, 0xf3, 0x27, 0xcd, 0x64, 0xa6, 0x2c, 0xf3, 0x5a, 0xbd, 0x2b, 0xa6, 0xfa, 0xb4 
	};

	gcm.set_key(aes_key);
	gcm.set_iv(iv);
	gcm.encrypt(plain_text, cipher_text);

	if (cipher_text == exp_cipher_text) {
		std::cout << "Cipher Text Corretto" << std::endl;
	} else {
		std::cout << "Cipher Text Errato" << std::endl;
		test_failed = 1;
	}

	std::array<uint8_t, 16> tag = gcm.get_tag();
	if (tag == exp_tag) {
		std::cout << "Tag Corretto" << std::endl;
	}
	else {
		std::cout << "Tag Errato" << std::endl;
		test_failed = 1;
	}
	/*std::cout << "PlainText: ";
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
	std::cout << std::endl;*/

	AES_GCM gcm_dec;
	gcm_dec.set_key(aes_key);
	gcm_dec.set_iv(iv);
	gcm_dec.set_tag(tag);
	std::vector<uint8_t> pl_received(cipher_text.size());
	gcm_dec.decrypt(cipher_text, pl_received);
	if (pl_received == plain_text) {
		std::cout << "Decrypt Corretta" << std::endl;
	}
	else {
		std::cout << "Decrypt Errata" << std::endl;
		test_failed = 1;
	}
	/*std::cout << "PlainText Received: ";
	for (auto byte : pl_received) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}*/

	return test_failed;
}
