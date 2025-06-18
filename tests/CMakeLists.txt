#TEST 1
add_executable(test_key "key_expansion_test.cpp" )
target_link_libraries(test_key PRIVATE 
	OpenCL_crypto_lib
)  
add_test(NAME test_key COMMAND test_key)

#TEST 2
add_executable(test_enc "encrypt_test.cpp")
target_link_libraries(test_enc PRIVATE 
	OpenCL_crypto_lib
)  
add_test(NAME test_enc COMMAND test_enc)

#TEST 3
add_executable(speed_test "speed_test.cpp")
target_link_libraries(speed_test PRIVATE 
	OpenCL_crypto_lib
)  
add_test(NAME speed_test COMMAND speed_test)

#TEST 4
add_executable(ghash_test "ghash_test.cpp")
target_link_libraries(ghash_test PRIVATE 
	OpenCL_crypto_lib
)  
add_test(NAME ghash_test COMMAND ghash_test)

#TEST 5
add_executable(gcm_test "gcm_test.cpp")
target_link_libraries(gcm_test PRIVATE 
	OpenCL_crypto_lib
)  
add_test(NAME gcm_test COMMAND gcm_test)