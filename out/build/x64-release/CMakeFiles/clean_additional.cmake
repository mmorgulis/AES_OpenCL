# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "AES_OpenCL_autogen"
  "CMakeFiles\\AES_OpenCL_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\AES_OpenCL_autogen.dir\\ParseCache.txt"
  "src\\CMakeFiles\\OpenCL_crypto_lib_autogen.dir\\AutogenUsed.txt"
  "src\\CMakeFiles\\OpenCL_crypto_lib_autogen.dir\\ParseCache.txt"
  "src\\OpenCL_crypto_lib_autogen"
  "tests\\CMakeFiles\\gcm_test_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\gcm_test_autogen.dir\\ParseCache.txt"
  "tests\\CMakeFiles\\ghash_test_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\ghash_test_autogen.dir\\ParseCache.txt"
  "tests\\CMakeFiles\\speed_test_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\speed_test_autogen.dir\\ParseCache.txt"
  "tests\\CMakeFiles\\test_enc_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\test_enc_autogen.dir\\ParseCache.txt"
  "tests\\CMakeFiles\\test_key_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\test_key_autogen.dir\\ParseCache.txt"
  "tests\\gcm_test_autogen"
  "tests\\ghash_test_autogen"
  "tests\\speed_test_autogen"
  "tests\\test_enc_autogen"
  "tests\\test_key_autogen"
  )
endif()
