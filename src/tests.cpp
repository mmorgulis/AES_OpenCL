#include <iostream>
#include <cstdint>
#include <vector>
#include "utils/safe_allocator.hpp"
#include <botan/hex.h>

void safe_allocator_test() {
	crypto::safe_vector<uint8_t> key;
	// The safe allocator of type T is used automatically
	// Alloc 16 byte by default (empty vector dimension)
	
}

int main() {
	safe_allocator_test();

	return 0;

}