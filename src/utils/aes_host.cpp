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

std::string loadSourceCode() {
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

void aes_encrypt(crypto::safe_vector<uint8_t> &plain_text, crypto::safe_vector<uint8_t> &cipher_text, 
				crypto::safe_vector<uint8_t> &round_keys) {

	cipher_text.resize(plain_text.size());
	std::string sourceCode = loadSourceCode();

	// Context
	cl::Context context(device);
	cl::CommandQueue queue(context, device);

	// Program
	cl::Program::Sources sources;
	sources.push_back({sourceCode.c_str(), sourceCode.length()});

	cl::Program program(context, sources);
	cl_int err_pr = program.build({device}, aes_version_define.c_str());

	if (err_pr != CL_SUCCESS) {
		std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		std::cerr << "Build failed with error code " << err_pr << "\n";
		std::cerr << "Build log:\n" << build_log << '\n';
	}

	// Kernel creation to encrypt
	cl::Kernel encrypt(program, "encrypt");
	
	// OpenCL buffers for plaintext and chipertext
	cl::Buffer plaintext(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, plain_text.size(), plain_text.data());
	cl::Buffer ciphertext(context, CL_MEM_READ_WRITE, cipher_text.size());
	cl::Buffer roundkey(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, round_keys.size(), round_keys.data());
	// Kernel functor
	cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> encrypter(encrypt);
	// Execute the function
	size_t global_work_size = 1; // parallel work 16 = bytes matrix
	encrypter(cl::EnqueueArgs(queue, cl::NDRange(global_work_size)), plaintext, ciphertext, roundkey);
	// Read data
	queue.enqueueReadBuffer(ciphertext, CL_TRUE, 0, cipher_text.size(), cipher_text.data());	
	queue.finish();
}

void aes_decrypt(crypto::safe_vector<uint8_t> &cipher_text, crypto::safe_vector<uint8_t> &plain_text,
	crypto::safe_vector<uint8_t>& inv_round_keys) {
	plain_text.resize(cipher_text.size());
	std::string sourceCode = loadSourceCode();
	// Context
	cl::Context context(device);
	cl::CommandQueue queue(context, device);

	// Program
	cl::Program::Sources sources;
	sources.push_back({sourceCode.c_str(), sourceCode.length()});

	cl::Program program(context, sources);
	cl_int err_pr = program.build({device}, aes_version_define.c_str());

	if (err_pr != CL_SUCCESS) {
		std::string build_log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device);
		std::cerr << "Build failed with error code " << err_pr << "\n";
		std::cerr << "Build log:\n" << build_log << '\n';
	}
	cl::Kernel decrypt(program, "decrypt");

	cl::Buffer ciphertext(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, cipher_text.size(), cipher_text.data());
	cl::Buffer plaintext(context, CL_MEM_READ_WRITE, plain_text.size());
	cl::Buffer inv_roundkey(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, inv_round_keys.size(), inv_round_keys.data());

	cl::KernelFunctor<cl::Buffer, cl::Buffer, cl::Buffer> decypter(decrypt);
	size_t global_work_size = 1; // parallel work 16 = bytes matrix
	decypter(cl::EnqueueArgs(queue, cl::NDRange(global_work_size)), ciphertext, plaintext, inv_roundkey);

	queue.enqueueReadBuffer(plaintext, CL_TRUE, 0, plain_text.size(), plain_text.data());	
	queue.finish();

}