#ifndef AES_HOST_H
#define AES_HOST_H

#define CL_TARGET_OPENCL_VERSION 300
#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <CL/opencl.hpp> // cl::
#include "aes_core.h"
#include "safe_allocator.hpp" // crypto
#include <span>
#include <string>


class AESOpenCL {

private:
	// OpenCL objects
	cl::Platform _platform;
	cl::Device _device;
	cl::Context _context;
	cl::CommandQueue _queue;
	cl::Program _program;
	cl::Kernel _encrypt_kernel;
	cl::Kernel _decrypt_kernel;
	
	cl::Buffer _plaintext_buf;
	cl::Buffer _ciphertext_buf;
	
	size_t _max_buffer_size = 128 * 1024 * 1024; // 128MB
	unsigned int _round_key_size = num_round_keys * 16;
	

public:
	AESOpenCL();
	~AESOpenCL() = default;

	bool find_platforms();
	/*void set_num_blocks(unsigned int num_blocks);
	unsigned int get_num_blocks();*/
	void set_input_size_in_mega_bytes(unsigned int mb);

	std::string loadSourceCode();

	// Communicates with OpenCL to encrypt
	void aes_encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text,
		std::span<const uint8_t> round_keys);

	// Communicates with OpenCL to encrypt
	void aes_decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text,
		std::span< const uint8_t> round_keys);

	cl::Context& get_context();
	cl::CommandQueue& get_queue();
	cl::Device& get_device();
	cl::Program& get_program();
	cl::Kernel& get_encrypt_kernel();
	cl::Kernel& get_decrypt_kernel();
	cl::Buffer& get_plaintext_buffer();
	cl::Buffer& get_ciphertext_buffer();
};

class CTROpenCL {
private:
	AESOpenCL _aes;
	cl::Kernel _encrypt_ctr_kernel;
	cl::Kernel _decrypt_ctr_kernel;
	cl::Buffer _iv_buf;
	unsigned int _iv_size = 12;
	size_t _max_buffer_size = 128 * 1024 * 1024; // 128MB
	unsigned int _round_key_size = num_round_keys * 16;

public:
	CTROpenCL();
	~CTROpenCL() = default;
	void aes_ctr_encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text,
		std::span<const uint8_t> round_keys, std::span<const uint8_t> iv);

	AESOpenCL& get_aes();

};
	
#endif // AES_HOST_H