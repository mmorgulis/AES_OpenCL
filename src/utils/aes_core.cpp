#include "aes_core.h"
#include <array>
#include <botan/auto_rng.h>
#include <botan/hex.h>
#include <botan/system_rng.h>
#include <numeric>


void generate_aes_key(crypto::safe_vector<uint8_t> &aes_key) {
	// Use random generator from Botan
	aes_key.resize(key_length / 8);
	std::unique_ptr<Botan::RandomNumberGenerator> rng;
	#if defined (BOTAN_HAS_SYSTEM_RNG)
		rng.reset(new Botan::System_RNG);
	#else
		rng.reset(new Botan::AutoSeeded_RNG);
	#endif
	// Generate pseudo-random number
	rng->randomize(aes_key.data(), aes_key.size());	
}

// Implementation followed : https://www.samiam.org/key-schedule.html
// Every round key (of every key_length) is 16 bytes
void key_schedule(crypto::safe_vector<uint8_t>& key, crypto::safe_vector<uint8_t>& round_keys) {
	unsigned int byte_copied, num_curr_round;
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

// No copy needed
std::span<const uint8_t> select_round_key(std::span<const uint8_t> round_keys, unsigned int num_key) {
    size_t offset = 16 * num_key;

    if (offset + 16 > round_keys.size()) {
        throw std::out_of_range("Indice oltre i limiti");
    }

    return round_keys.subspan(offset, 16); 
}

crypto::safe_vector<uint8_t> invert_round_keys(crypto::safe_vector<uint8_t>& round_keys) {
	//return { round_keys.rbegin(), round_keys.rend() };
	return crypto::safe_vector<uint8_t>(round_keys.rbegin(), round_keys.rend());
}