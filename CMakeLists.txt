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

if(NOT DEFINED ENV{Botan_DIR})
  message(FATAL_ERROR "La variabile d'ambiente 'Botan_DIR' non è definita.")
endif()
set(Botan_DIR "$ENV{Botan_DIR}")

find_package(Botan REQUIRED)
#message(STATUS "Prefix ${_Botan_PREFIX}")
#message(STATUS "Lib ${_Botan_shared_lib}")

# Aggiungi una libreria INTERFACE per OpenCL
add_library(OpenCL INTERFACE)

# Specifica dove trovare header e libreria per OpenCL
target_include_directories(OpenCL INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(OpenCL INTERFACE ${CMAKE_SOURCE_DIR}/lib/OpenCL.lib)

# Colleziona i sorgenti
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
    "include/*.cpp"
    "include/*.hpp"
    "include/*.h"
)   

# Crea l'eseguibile
add_executable(OpenCL_cryptography ${SOURCES})

# Collega OpenCL
target_link_libraries(OpenCL_cryptography PUBLIC OpenCL)
target_link_libraries(OpenCL_cryptography PUBLIC Botan::Botan)

# Imposta una variabile per il path alla DLL
if(NOT DEFINED ENV{Botan_LIB})
  message(FATAL_ERROR "La variabile d'ambiente 'Botan_LIB' non è definita.")
endif()

set(BOTAN_DLL_PATH "$ENV{Botan_LIB}/botan-3.dll")

if(NOT EXISTS "${BOTAN_DLL_PATH}")
  message(FATAL_ERROR "File DLL non trovato: ${BOTAN_DLL_PATH}")
endif()

# Copia i kernels e il dll dopo la build
# La dll è necessaria sia nella cartella di build che in quella di test
add_custom_command(TARGET OpenCL_cryptography POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${BOTAN_DLL_PATH}"
            $<TARGET_FILE_DIR:OpenCL_cryptography>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${BOTAN_DLL_PATH}"
            "$<TARGET_FILE_DIR:OpenCL_cryptography>/tests"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/kernels
            $<TARGET_FILE_DIR:OpenCL_cryptography>/kernels
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/kernels
            "$<TARGET_FILE_DIR:OpenCL_cryptography>/tests/kernels"
)

# Abilita i test
enable_testing()

# Aggiungi la directory del src (per lib)
add_subdirectory(src)

# Aggiungi la directory dei test
add_subdirectory(tests)

# Messaggio finale
message(STATUS "OpenCL_cryptography configurato")
message(STATUS "Version : ${Botan_VERSION}")
message(STATUS "Found : ${Botan_FOUND}")