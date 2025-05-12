#define CL_TARGET_OPENCL_VERSION 300
#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <botan/auto_rng.h>
#include <botan/hex.h>
#include <botan/system_rng.h>
#include <CL/cl.h>
#include <CL/opencl.hpp>
#include <cstdint>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "utils/safe_allocator.hpp"

// Fill devices_choosen if the function find a device
static bool find_platforms(cl::Platform &platform_chosen, cl::Device &device_chosen) noexcept{
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




int main() {
	// OpenCL platforms and devices
	cl::Platform platform;
	cl::Device device;
	if (!find_platforms(platform, device)) {
		std::cout << "No platforms detected\n";
		return -1;
	}
	std::cout << "Platform chosen: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";
	std::cout << "Device chosen: " << device.getInfo<CL_DEVICE_NAME>() << "\n";

	// Context
	cl::Context context(device);
	cl::CommandQueue queue(context, device);

	// Macro definition for key length
	unsigned int key_length = 32; //Default key length
	unsigned int num_round_keys = 15; //Rounds key for AES-256
	std::cout << "Pick the key length between 16, 24, 32\n";
	std::cin >> key_length;

	std::string aes_version_define;
	switch (key_length) {
	case 16: 
		aes_version_define = "-DAES_128"; 
		num_round_keys = 11;
		break;
	case 24: 
		aes_version_define = "-DAES_192"; 
		num_round_keys = 13; 
		break;
	case 32: 
		aes_version_define = "-DAES_256"; 
		break;
	default: 
		std::cout << "Unsupported Key Length \n"; 
		return -1;
	}

	// Bytes for round_keys array
	unsigned int round_keys_size = num_round_keys * key_length; // bytes
	crypto::safe_vector<uint8_t> round_keys(num_round_keys);
	
	// Generate key
	std::unique_ptr<Botan::RandomNumberGenerator> rng;
	#if defined (BOTAN_HAS_SYSTEM_RNG)
		rng.reset(new Botan::System_RNG);
	#else
		rng.reset(new Botan::AutoSeeded_RNG);
	#endif

	crypto::safe_vector<uint8_t> aes_key(key_length);
	// The safe allocator of type T is used automatically
	// Alloc 16 byte by default (empty vector dimension)

	rng->randomize(aes_key.data(), aes_key.size());
	/*
	for (uint8_t byte : aes_key) {
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << '\n'; 
	*/
	key_schedule(aes_key, round_keys);

	// Program
	// Load source code
	std::ifstream file("aes_kernel.cl");
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
	
	
	
	cl::Kernel decrypt(program, "decrypt_n");
	



	queue.finish();

	return 0;

}