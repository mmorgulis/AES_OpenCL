#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <filesystem>

#include "aes_core.h"
#include "aes_host.h"
#include "safe_allocator.hpp"

bool find_platforms(cl::Platform& platform_chosen, cl::Device& device_chosen) noexcept {
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
		platform_chosen = temp_platform;
		device_chosen = temp_device;
		return true;
	} else {
		return false;
	}
}

void aes_encrypt(crypto::safe_vector<uint8_t> plain_text, crypto::safe_vector<uint8_t> cipher_text) {
	unsigned int num_blocks = plain_text.size() % 16; // 128 bit = block size
	
	// Context
	cl::Context context(device);
	cl::CommandQueue queue(context, device);

	// Program
	// Load source code
	std::filesystem::path kernel_path = std::filesystem::current_path() / "aes_kernel.cl";
	std::ifstream file(kernel_path);
	if (!file.is_open()) {
		throw std::runtime_error("Impossibile aprire il file aes_kernel.cl");
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	std::string sourceCode = oss.str();

	cl::Program::Sources sources;
	sources.push_back({sourceCode.c_str(), sourceCode.length()});

	cl::Program program(context, sources);
	cl_int err_pr = program.build({device}, aes_version_define.c_str());

	// Create kernels for specific functions
	/*
	cl::Kernel round_key_gen(program, "key_derivation_funct");
	// OpenCL Buffers for the key
	cl::Buffer key_buf(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, aes_key.size(), aes_key.data());
	cl::Buffer round_keys_buf(context, CL_MEM_READ_WRITE, round_keys_size);
	// Kernel functor
	cl::KernelFunctor<cl::Buffer, cl::Buffer> key_expander(round_key_gen);
	// Execute the function
	size_t global_work_size = round_keys_size; // parallel work
	key_expander(cl::EnqueueArgs(queue, cl::NDRange(global_work_size)), key_buf, round_keys_buf);
	// Read data
	queue.enqueueReadBuffer(round_keys_buf, CL_TRUE, 0, round_keys_size, round_keys.data());
	*/


	cl::Kernel encrypt(program, "encrypt_n");
	// OpenCL buffers for plaintext and chipertext
	cl::Buffer plaintext(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(uint8_t) * plain_text.size(), plain_text.data());
	
	queue.finish();
}

void aes_decrypt(crypto::safe_vector<uint8_t> plain_text, crypto::safe_vector<uint8_t> cipher_text) {

}