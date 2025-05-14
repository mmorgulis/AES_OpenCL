#include "aes_core.h"

#include <array>
#include <numeric>


// Implementation followed : https://www.samiam.org/key-schedule.html
// Every round key (of every key_length) is 16 bytes
void key_schedule(crypto::safe_vector<uint8_t>& key, crypto::safe_vector<uint8_t>& round_keys,
					unsigned int key_length) {
	switch (key_length) {
	case 128:
		round_keys.resize(176); // 11 round keys of 16 bytes = 176 bytes
		// Copy the first 16 bytes from the key
		std::copy_n(key.begin(), 16, std::back_inserter(round_keys));
		unsigned int byte_copied = 16;
		unsigned int num_round = 1;
		std::array<uint8_t, 4> t = {};
		while (byte_copied < 176) { // 11 rounds
			std::copy_n(round_keys.begin() + byte_copied - 4, 4, t.begin());
			// Every 16 bytes (every round), do a complex calculation 
			if (byte_copied % 16 == 0) {
				schedule_core(t, num_round);
				++num_round;
			}	
			for (unsigned int i = 0; i < 4; ++i) {
				round_keys[byte_copied] = round_keys[byte_copied - 16] ^ t[i];
			}
			
		}
		break;

	case 192:
		round_keys.resize(208);
		std::copy_n(key.begin(), 24, std::back_inserter(round_keys));
		unsigned int byte_copied = 24;
		unsigned int num_round = 1;
		std::array<uint8_t, 4> t = {};
		while (byte_copied < 208) { // 13 rounds
			std::copy_n(round_keys.begin() + byte_copied - 4, 4, t.begin());
			// Every 24 bytes (every round), do a complex calculation 
			if (byte_copied % 24 == 0) {
				schedule_core(t, num_round);
				++num_round;
			}	
			for (unsigned int i = 0; i < 4; ++i) {
				round_keys[byte_copied] = round_keys[byte_copied - 24] ^ t[i];
			}
			
		}
		break;

	case 256:
		round_keys.resize(240);
		std::copy_n(key.begin(), 32, std::back_inserter(round_keys));
		unsigned int byte_copied = 24;
		unsigned int num_round = 1;
		std::array<uint8_t, 4> t = {};
		while (byte_copied < 208) { // 15 rounds
			std::copy_n(round_keys.begin() + byte_copied - 4, 4, t.begin());
			// Every 24 bytes (every round), do a complex calculation 
			if (byte_copied % 32 == 0) {
				schedule_core(t, num_round);
				++num_round;
			}	
			for (unsigned int i = 0; i < 4; ++i) {
				round_keys[byte_copied] = round_keys[byte_copied - 32] ^ t[i];
			}
			
		}
		break;


	default:
		break;
	}
}

void schedule_core(std::array<uint8_t, 4> t, unsigned int num_round) {

}
