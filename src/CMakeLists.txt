add_library(OpenCL_crypto_lib STATIC ${SOURCES})

target_include_directories(OpenCL_crypto_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OpenCL_crypto_lib
    PUBLIC
    Botan::Botan
    OpenCL
)