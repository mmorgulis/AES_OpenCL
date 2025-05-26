#include "aes_core.h"
#include <array>
#include <botan/auto_rng.h>
#include <botan/hex.h>
#include <botan/system_rng.h>
#include <numeric>

void set_aes_parameters(unsigned int aes_version) {
	switch (aes_version) {
	case 128:
		key_length = 128;
		aes_version_define = "-DAES_128";
		num_round_keys = 11;
		num_rounds = 10;
		break;
	case 192:
		key_length = 192;
		aes_version_define = "-DAES_192";
		num_round_keys = 13;
		num_rounds = 12;
		break;
	case 256:
		key_length = 256;
		aes_version_define = "-DAES_256";
		num_round_keys = 15;
		num_rounds = 14;
		break;
	}
}

crypto::safe_vector<uint8_t> generate_aes_key() {
	crypto::safe_vector<uint8_t> aes_key(key_length/8);
	set_aes_parameters(key_length);
	// Use random generator from Botan
	std::unique_ptr<Botan::RandomNumberGenerator> rng;
	#if defined (BOTAN_HAS_SYSTEM_RNG)
		rng.reset(new Botan::System_RNG);
	#else
		rng.reset(new Botan::AutoSeeded_RNG);
	#endif
	// Generate pseudo-random number
	rng->randomize(aes_key.data(), aes_key.size());	
	return aes_key;
}

// Implementation followed : https://www.samiam.org/key-schedule.html
// Every round key (of every key_length) is 16 bytes
crypto::safe_vector<uint8_t> key_schedule(std::span<const uint8_t> key) {
	// Safety control
	if (key.size() != 16 && key.size() != 24 && key.size() != 32) {
		throw std::invalid_argument("Invalid Key Length");
	}
	unsigned int byte_copied, num_curr_round;
	crypto::safe_vector<uint8_t> round_keys;
	std::array<uint8_t, 4> t = {};
	switch (key_length) {
	case 128:
		round_keys.resize(176); // 11 round keys of 16 bytes = 176 bytes
		// Copy the first 16 bytes from the key
		std::copy_n(key.begin(), 16, round_keys.begin());
		byte_copied = 16;
		num_curr_round = 0;
		while (byte_copied < 176) { // 11 keys
			std::copy_n(round_keys.begin() + byte_copied - 4, 4, t.begin());
			// Every 16 bytes (every round), do rotate-subword-rcon
			if (byte_copied % 16 == 0) {
				schedule_core(t, num_curr_round);
				++num_curr_round;
			}	
			// XOR
			for (unsigned int i = 0; i < 4; ++i) {
				round_keys[byte_copied] = round_keys[byte_copied - 16] ^ t[i];
				++byte_copied;
			}			
		}
		break;

	case 192:
		round_keys.resize(208);
		std::copy_n(key.begin(), 24, round_keys.begin());
		byte_copied = 24;
		num_curr_round = 0;
		while (byte_copied < 208) { // 13 keys
			std::copy_n(round_keys.begin() + byte_copied - 4, 4, t.begin());
			// Every 24 bytes (every round) 
			if (byte_copied % 24 == 0) {
				schedule_core(t, num_curr_round);
				++num_curr_round;
			}	
			for (unsigned int i = 0; i < 4; ++i) {
				round_keys[byte_copied] = round_keys[byte_copied - 24] ^ t[i];
				++byte_copied;
			}	
		}
		break;

	case 256:
		round_keys.resize(240);
		std::copy_n(key.begin(), 32, round_keys.begin());
		byte_copied = 32;
		num_curr_round = 0;
		while (byte_copied < 240) { // 15 keys
			std::copy_n(round_keys.begin() + byte_copied - 4, 4, t.begin());
			// Every 32 bytes (every round)
			if (byte_copied % 32 == 0) {
				schedule_core(t, num_curr_round);
				++num_curr_round;
			}
			// For 256-bit keys, we add an extra sbox to the calculations
			if(byte_copied % 32 == 16) {
                    for(unsigned int i = 0; i < 4; i++) 
                            t[i] = SBox[t[i]];
                }
			for (unsigned int i = 0; i < 4; ++i) {
				round_keys[byte_copied] = round_keys[byte_copied - 32] ^ t[i];
				++byte_copied;
			}
			
		}
		break;

	default:
		break;
	}
	return round_keys;
}

void schedule_core(std::array<uint8_t, 4> &t, unsigned int num_curr_round) {
	// Rotate
	rotate(t);
	// SubWord (apply sbox = sostituation block) to all 4 bytes)
	for (unsigned int i = 0; i < 4; ++i) {
		t[i] = SBox[t[i]];
	}
	// On just the first byte, add 2^i to the byte
	t[0] ^= Rcon[num_curr_round];
}

void rotate(std::array<uint8_t, 4>& t) {
	// Save first byte
	uint8_t tmp = t[0];
	// Rotate other 3 and finally assign tmp to the last
	for (unsigned int i = 0; i < 3; ++i) {
		t[i] = t[i + 1];
	}
	t[3] = tmp;
}