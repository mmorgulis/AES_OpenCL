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

## Hardware Requirements
- A compatible OpenCL is required, it's strongly recommended to use a device with sufficient memory (e.g., **dedicated GPU**), otherwise the program may crash or exit unexpectedly

## Notes
- The project is developed using VS2022 and MSCV with CMake build, it has not been tested for other platform/configurations.
- It uses QT 6.9.1 (for MSCV) and Botan 3.8.1, so, to test the project, make sure is all installed and accessible via enviroment variables (for example mine are Botan_DIR: "D:\Botan-3.8.1\build-output\lib\cmake\Botan-3.8.1" and Qt6_DIR_: "D:\Qt\6.9.1\msvc2022_64\lib\cmake\Qt6".
- The project is intended for educational and research purposes; it is not production-ready for secure communications.


## Build for Windows
Install Botan, Qt, (if you are on VS) Qt Visual Studio Tools and define environment variables;
1. cmake -S . -B build -DCMAKE_INSTALL_PREFIX=out
2. cmake --build build --config Release
3. cmake --install build --config Release
4. windeployqt out/bin/AES_OpenCL.exe
