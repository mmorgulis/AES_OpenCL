cmake_minimum_required(VERSION 3.10)

# Abilita ricaricamento rapido per MSVC, se supportato
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Nome progetto
project(OpenCL_cryptography LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#set(BOTAN_ROOT "D:/Botan-3.7.1/build_output")
set(Botan_DIR "D:/Botan-3.7.1/build_output/lib/cmake/Botan-3.7.1")
#include_directories(${BOTAN_ROOT}/include)
#link_directories(${BOTAN_ROOT}/lib)

find_package(Botan)

# Aggiungi una libreria INTERFACE per OpenCL
add_library(OpenCL INTERFACE)

# Specifica dove trovare header e libreria per OpenCL
target_include_directories(OpenCL INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(OpenCL INTERFACE ${CMAKE_SOURCE_DIR}/lib/OpenCL.lib)

# Colleziona i sorgenti
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    src/*.cpp
)

# Crea l'eseguibile
add_executable(OpenCL_cryptography ${SOURCES} "src/tests.cpp")

# Collega OpenCL
target_link_libraries(OpenCL_cryptography PRIVATE OpenCL)
target_link_libraries(OpenCL_cryptography PRIVATE Botan::Botan-static)

# Copia i kernels dopo la build
add_custom_command(TARGET OpenCL_cryptography POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/kernels $<TARGET_FILE_DIR:OpenCL_cryptography>/kernels
)

# Messaggio finale
message(STATUS "OpenCL_cryptography configurato")
message(STATUS "Version : ${Botan_VERSION}")
message(STATUS "Found : ${Botan_FOUND}")


