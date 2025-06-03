#include <botan/auto_rng.h>
#include <botan/hex.h>
#include <botan/ctr.h>
#include <botan/aes.h>
#include <botan/symkey.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <chrono>
#include <span>
#include "../src/utils/aes_core.h"
#include "../src/utils/aes_host.h"
#include "utils/aes_mode.h"
#include "../src/utils/aes_ctr.h"
#include "../src/utils/safe_allocator.hpp"

std::string generate_test_string(size_t size_in_megabytes) {
    std::string pattern = "Lorem ipsum dolor sit amet, consectetur adipiscing elit, "
        "sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. "
        "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris "
        "nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";
    std::string result;
    result.reserve(size_in_megabytes * 1024 * 1024);
    while (result.size() + pattern.size() < size_in_megabytes * 1024 * 1024) {
        result += pattern;
    }
    result.append(size_in_megabytes * 1024 * 1024 - result.size(), 'X');
    return result;
}

int main() {
    const std::size_t SIZE_MB = 128;
    const std::string plain_text = generate_test_string(SIZE_MB);
    std::vector<uint8_t> cipher_text(plain_text.size());

    crypto::safe_vector<uint8_t> aes_key = generate_aes_key(256);
    crypto::safe_vector<uint8_t> round_keys = key_schedule(aes_key);

    AES_CTR ctr;
    ctr.set_key(aes_key);
    std::array<uint8_t, 12> iv = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 'a', 'b', 'c' };
    ctr.set_iv(iv);

    auto start = std::chrono::high_resolution_clock::now();
    ctr.encrypt(
        std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(plain_text.data()), plain_text.size()),
        std::span<uint8_t>(cipher_text.data(), cipher_text.size())
    );
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    double throughput_bytes = plain_text.size() / elapsed.count();
    double throughput_MB = throughput_bytes / (1024.0 * 1024.0);

    std::cout << "Time passed (AES CTR): " << elapsed.count() << " seconds\n";
    std::cout << "Throughput ENCRYPT: " << throughput_MB << " MB/sec\n";

    Botan::AutoSeeded_RNG rng;
    Botan::SymmetricKey key_bt(rng, 32);
    Botan::InitializationVector iv_bt(rng, 12);
    std::unique_ptr<Botan::StreamCipher> cipher(Botan::StreamCipher::create("CTR(AES-256)"));
    if (!cipher) {
        std::cerr << "Impossibile creare il cifrario CTR(AES-256)\n";
        return 1;
    }

    cipher->set_key(key_bt);
    cipher->set_iv(iv_bt);
    std::vector<uint8_t> buffer(plain_text.begin(), plain_text.end());

    auto start_bt = std::chrono::high_resolution_clock::now();
    cipher->encrypt(buffer);
    auto end_bt = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_bt = end_bt - start_bt;
    double throughput_Bps = buffer.size() / elapsed_bt.count();
    double throughput_MBps = throughput_Bps / (1024.0 * 1024.0);

    std::cout << "Time passed (Botan AES CTR): " << elapsed_bt.count() << " seconds\n";
    std::cout << "Throughput ENCRYPT (Botan): " << throughput_MBps << " MB/sec\n";

    return 0;
}
