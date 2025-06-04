#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath> 
#include <filesystem>
#include "aes_core.h"
#include "aes_host.h"
#include "safe_allocator.hpp"

AESOpenCL::AESOpenCL() {
	if (!find_platforms())
		throw std::runtime_error("No OPENCL platforms detected");

	// Context
	_context = cl::Context(_device);
	_queue = cl::CommandQueue(_context, _device);

	// Program
	std::string sourceCode = loadSourceCode();
	cl::Program::Sources sources;
	sources.push_back({ sourceCode.c_str(), sourceCode.length() });

	_program = cl::Program(_context, sources);
	cl_int err_pr = _program.build({ _device });

	if (err_pr != CL_SUCCESS) {
		std::string build_log = _program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_device);
		std::cerr << "Build failed with error code " << err_pr << "\n";
		std::cerr << "Build log:\n" << build_log << '\n';
	}

	// Kernel creation
	_encrypt_kernel = cl::Kernel(_program, "encrypt_n");
	_decrypt_kernel = cl::Kernel(_program, "decrypt_n");

	// Buffer preallocation
	_plaintext_buf = cl::Buffer(_context, CL_MEM_READ_WRITE, _max_buffer_size);
	_ciphertext_buf = cl::Buffer(_context, CL_MEM_READ_WRITE, _max_buffer_size);
	
}

//void AESOpenCL::set_num_blocks(unsigned int num_blocks) {
//	_num_blocks = num_blocks;
//}
//
//unsigned int AESOpenCL::get_num_blocks()
//{
//	return _num_blocks;
//}

void AESOpenCL::set_input_size_in_mega_bytes(unsigned int mb)
{
	_max_buffer_size = (size_t) mb * 1024 * 1024;
}

bool AESOpenCL::find_platforms() {
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	cl_int res = cl::Platform::get(&platforms);
	std::string name, vendor, version, profile, extensions;
	if (res == CL_SUCCESS) {
		//std::cout << "Piattaform OpenCL presenti\n";
		cl_ulong best_memory = 0;
		bool found_device = false;
		unsigned int platform_count = 0;
		cl::Device temp_device;
		cl::Platform temp_platform;
		for (const cl::Platform& platform : platforms) {
			/*
			platform.getInfo(CL_PLATFORM_NAME, &name);
			platform.getInfo(CL_PLATFORM_VENDOR, &vendor);
			platform.getInfo(CL_PLATFORM_VERSION, &version);
			platform.getInfo(CL_PLATFORM_PROFILE, &profile);
			platform.getInfo(CL_PLATFORM_EXTENSIONS, &extensions);

			std::cout << "Piattaforma #" << ++platform_count << ":\n";
			std::cout << "  Nome:    " << name << "\n";
			std::cout << "  Vendor:  " << vendor << "\n";
			std::cout << "  Version: " << version << "\n";
			std::cout << "  Profile: " << profile << "\n";
			*/
			
			cl_int res_dev = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
			// Loop on devices and pick the one with bigger memory
			for (const cl::Device& device : devices) {
				cl_ulong memory;
				device.getInfo(CL_DEVICE_GLOBAL_MEM_SIZE, &memory);	
				if (!found_device || memory > best_memory) {
					temp_device  = device;
					temp_platform = platform;
					best_memory = memory;
					found_device = true;
				}
			}
		}
		_platform = temp_platform;
		_device = temp_device;
		return true;
	} else {
		return false;
	}
}

std::string AESOpenCL::loadSourceCode() {
	// Load source code
	std::filesystem::path kernel_path = std::filesystem::current_path() / "kernels" / "aes_kernel.cl";
	std::ifstream file(kernel_path);
	if (!file.is_open()) {
		throw std::runtime_error("Impossibile aprire il file aes_kernel.cl");
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	std::string sourceCode = oss.str();
	return sourceCode;

}

void AESOpenCL::aes_encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text, 
				std::span<const uint8_t> round_keys) {
	// Safety control
	if (cipher_text.size() != plain_text.size()) {
        throw std::invalid_argument("Size error");
    }
	if (plain_text.size() > _max_buffer_size) {
		throw std::invalid_argument("Input too large for preallocated buffer");
	}

	unsigned int num_blocks = (unsigned int) plain_text.size() / 16;

	// Create round key buffer (small and not fixed dimension)
	cl::Buffer round_keys_buf(_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, round_keys.size(), (void*)round_keys.data());
	
	// Write in preallocated buffer
	_queue.enqueueWriteBuffer(_plaintext_buf, CL_TRUE, 0, plain_text.size(), plain_text.data());

	// Kernel functor
	cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl_uint, cl_uint> encrypter(_encrypt_kernel);
	// Execute the function
	size_t global_work_size = num_blocks; // parallel work 1 work group for 1 aes block
	cl_uint n_blocks = num_blocks;
	cl_uint n_rounds = ((unsigned int) round_keys.size() / 16) - 1;
	//<std::cout << "Num WG: " << global_work_size << std::endl;
	encrypter(cl::EnqueueArgs(_queue, cl::NDRange(global_work_size)),
		_plaintext_buf, _ciphertext_buf, round_keys_buf, n_blocks, n_rounds);
	// Read data
	_queue.enqueueReadBuffer(_ciphertext_buf, CL_TRUE, 0, cipher_text.size(), cipher_text.data());
	_queue.finish();

	// Clear buffer
	std::vector<uint8_t> zero_buf(_max_buffer_size, 0);
	_queue.enqueueWriteBuffer(_plaintext_buf, CL_FALSE, 0, _max_buffer_size, zero_buf.data());
	_queue.enqueueWriteBuffer(_ciphertext_buf, CL_FALSE, 0, _max_buffer_size, zero_buf.data());
	_queue.enqueueWriteBuffer(round_keys_buf, CL_FALSE, 0, round_keys.size(), zero_buf.data());
}

void AESOpenCL::aes_decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text,
	std::span<const uint8_t> round_keys) {
	if (cipher_text.size() != plain_text.size()) {
        throw std::invalid_argument("Size error");
    }
	if (cipher_text.size() > _max_buffer_size) {
		throw std::invalid_argument("Input too large for preallocated buffer");
	}

	unsigned int num_blocks = (unsigned int) plain_text.size() / 16;

	// Allocate buffer for round keys
	cl::Buffer round_keys_buf(_context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, round_keys.size(), (void*)round_keys.data());
	
	// Write into preallocated buffer
	_queue.enqueueWriteBuffer(_ciphertext_buf, CL_TRUE, 0, cipher_text.size(), cipher_text.data());

	cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl_uint, cl_uint> decypter(_decrypt_kernel);
	size_t global_work_size = num_blocks; 
	cl_uint n_blocks = num_blocks;
	cl_uint n_rounds = ((unsigned int)round_keys.size() / 16) - 1;
	decypter(cl::EnqueueArgs(_queue, cl::NDRange(global_work_size)), _ciphertext_buf, _plaintext_buf, round_keys_buf, n_blocks, n_rounds);
	_queue.enqueueReadBuffer(_plaintext_buf, CL_TRUE, 0, plain_text.size(), plain_text.data());	
	_queue.finish();

	// Clear buffer
	std::vector<uint8_t> zero_buf(_max_buffer_size, 0);
	_queue.enqueueWriteBuffer(_plaintext_buf, CL_FALSE, 0, _max_buffer_size, zero_buf.data());
	_queue.enqueueWriteBuffer(_ciphertext_buf, CL_FALSE, 0, _max_buffer_size, zero_buf.data());
	_queue.enqueueWriteBuffer(round_keys_buf, CL_FALSE, 0, round_keys.size(), zero_buf.data());

}

cl::Context& AESOpenCL::get_context() { return _context; }
cl::CommandQueue& AESOpenCL::get_queue() { return _queue; }
cl::Device& AESOpenCL::get_device() { return _device; }
cl::Program& AESOpenCL::get_program() { return _program; }
cl::Kernel& AESOpenCL::get_encrypt_kernel() { return _encrypt_kernel; }
cl::Kernel& AESOpenCL::get_decrypt_kernel() { return _decrypt_kernel; }
cl::Buffer& AESOpenCL::get_plaintext_buffer() { return _plaintext_buf; }
cl::Buffer& AESOpenCL::get_ciphertext_buffer() { return _ciphertext_buf; }




CTROpenCL::CTROpenCL() {
	// Kernel ctr creation
	_encrypt_ctr_kernel = cl::Kernel(_aes.get_program(), "encrypt_ctr");

	_iv_buf = cl::Buffer(_aes.get_context(), CL_MEM_READ_ONLY, _iv_size);
}

void CTROpenCL::aes_ctr_encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text,
	std::span<const uint8_t> round_keys, std::span<const uint8_t> iv)
{
	if (cipher_text.size() != plain_text.size()) {
		throw std::invalid_argument("Size error");
	}
	if (plain_text.size() > _max_buffer_size) {
		throw std::invalid_argument("Input too large for preallocated buffer");
	}

	// Ceil blocks
	unsigned int num_blocks = (unsigned int)((plain_text.size() + 15) / 16);
	size_t padded_size = (size_t) num_blocks * 16;

	// Allocate round_key buffer
	cl::Buffer round_keys_buf(_aes.get_context(), CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, round_keys.size(), (void*)round_keys.data());

	// Preallocated buffer 
	_aes.get_queue().enqueueWriteBuffer(_aes.get_plaintext_buffer(), CL_TRUE, 0, plain_text.size(), plain_text.data());
	_aes.get_queue().enqueueWriteBuffer(_iv_buf, CL_TRUE, 0, iv.size(), iv.data());
	cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer, cl::Buffer, cl_uint, cl_uint> encrypter(_encrypt_ctr_kernel);

	// Padding
	if (padded_size > plain_text.size()) {
		std::vector<uint8_t> zeros(padded_size - plain_text.size(), 0);
		_aes.get_queue().enqueueWriteBuffer(_aes.get_plaintext_buffer(), CL_TRUE,
			plain_text.size(), zeros.size(), zeros.data());
	}

	size_t global_work_size = num_blocks; // parallel work 1 work group for 1 aes block
	cl_uint n_blocks = num_blocks;
	cl_uint n_rounds = ((unsigned int)round_keys.size() / 16) - 1;
	encrypter(cl::EnqueueArgs(_aes.get_queue(), cl::NDRange(global_work_size)),
		_aes.get_plaintext_buffer(), _aes.get_ciphertext_buffer(), round_keys_buf, _iv_buf, n_blocks, n_rounds);
	// Read data
	_aes.get_queue().enqueueReadBuffer(_aes.get_ciphertext_buffer(), CL_TRUE, 0, cipher_text.size(), cipher_text.data());
	_aes.get_queue().finish();

	// Clear buffer
	std::vector<uint8_t> zero_buf(_max_buffer_size, 0);
	_aes.get_queue().enqueueWriteBuffer(_aes.get_plaintext_buffer(), CL_FALSE, 0, _max_buffer_size, zero_buf.data());
	_aes.get_queue().enqueueWriteBuffer(_aes.get_ciphertext_buffer(), CL_FALSE, 0, _max_buffer_size, zero_buf.data());
	_aes.get_queue().enqueueWriteBuffer(round_keys_buf, CL_FALSE, 0, round_keys.size(), zero_buf.data());
	_aes.get_queue().enqueueWriteBuffer(_iv_buf, CL_FALSE, 0, _iv_size, zero_buf.data());
}


AESOpenCL& CTROpenCL::get_aes() { return _aes; }
