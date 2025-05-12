#include "safe_allocator.hpp"

/*
* @brief Function to create round keys
* 
* Every round keys depend on the previous one,
* so parallelitation is not so useful
* 
* @param key vector for aes key, length depending on the version of aes
* 
* @param key vector for round keys, every rows is a byte of the round key,
* the number of rows that rapresent a complete key depend on the version of aes
* The function return void but fill the round_keys vector
*		
*/

static void key_schedule(crypto::safe_vector<uint8_t>& key, crypto::safe_vector<uint8_t>& round_keys);