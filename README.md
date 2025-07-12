# OpenCL_Cryptography
This is a C++ thesis project that implements an AES encryption algorithm using OpenCL.  
The four fundamental AES operations — **AddRoundKey**, **SubBytes**, **ShiftRows**, and **MixColumns** — are implemented in row-major form using OpenCL.  
The project also supports two modes of operation: **CTR** (implemented in OpenCL) and **GCM**.

Additionally, it features a custom data structure called `safe_vector<T>`, based on `safe_allocator<T>`, designed to improve memory safety and manage sensitive data securely.

The project includes basic test cases, such as:

- A performance test for CTR mode.
- Correctness verification using official **NIST test vectors**.

The main executable is `AES_OpenCL.exe`. It uses **Qt** to simulate a two-node communication setup via `QTcpSocket`, running on two separate processes.

---

## Example Usage

```cpp
AES_GCM gcm;
crypto::safe_vector<uint8_t> aes_key = { ... };
crypto::safe_vector<uint8_t> plain_text = { ... };
std::vector<uint8_t> cipher_text(plain_text.size());
std::vector<uint8_t> iv = { ... }; // 16 bytes

gcm.set_key(aes_key);
gcm.set_iv(iv);
gcm.encrypt(plain_text, cipher_text); 
```
Or, for simpler usage:
```cpp
// gcm, key, and iv are already set
std::string plain_text = "...";
gcm.encrypt(plain_text); // Slower, but easier to use
```


## Notes
- The project is developed using VS2022 and MSCV with CMake build, it has not been tested for other platform/configurations.
- It uses QT 6.9.1 and Botan 3.8.1, so, to test the project, make sure is all installed and accessible via enviroment variables (e.g. Botan_DIR).
- The device must have an OpenCL device, otherwise the program crashes.
- The project is intended for educational and research purposes; it is not production-ready for secure communications.
