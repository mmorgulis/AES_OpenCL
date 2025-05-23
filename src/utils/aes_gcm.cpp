#include "aes_gcm.h"

AES_GCM::AES_GCM(crypto::safe_vector<uint8_t> round_keys) {
	// inizializzo tutti gli attributi casuali usando botan
	// cioè IV, tag, add ... con set_iv ecc
}