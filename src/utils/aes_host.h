#ifndef AES_HOST_H
#define AES_HOST_H

#define CL_TARGET_OPENCL_VERSION 300
#define CL_HPP_TARGET_OPENCL_VERSION 300

#include <CL/opencl.hpp> // cl::
#include "safe_allocator.hpp" // crypto
#include <span>

// OpenCL current device
inline cl::Platform platform;
inline cl::Device device;

// Fill devices_choosen if the function find a device
bool find_platforms(cl::Platform& platform_chosen, cl::Device& device_chosen) noexcept;

// Communicates with OpenCL to encrypt
void aes_encrypt(std::span<const uint8_t> plain_text, std::span<uint8_t> cipher_text,
	std::span<const uint8_t> round_keys);

// Communicates with OpenCL to encrypt
void aes_decrypt(std::span<const uint8_t> cipher_text, std::span<uint8_t> plain_text,
	std::span< const uint8_t> round_keys);

#endif // AES_HOST_H