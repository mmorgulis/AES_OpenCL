cmake_minimum_required(VERSION 3.10)

# Abilita ricaricamento rapido per MSVC, se supportato
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Nome progetto
project(AES_OpenCL LANGUAGES CXX)

# Standard C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED ENV{Qt6_DIR})
  message(FATAL_ERROR "La variabile d'ambiente 'Qt6_DIR' non è definita.")
endif()

if(NOT DEFINED ENV{Botan_DIR})
  message(FATAL_ERROR "La variabile d'ambiente 'Botan_DIR' non è definita.")
endif()
set(Botan_DIR "$ENV{Botan_DIR}")

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
find_package(Botan REQUIRED)
#message(STATUS "Prefix ${_Botan_PREFIX}")
#message(STATUS "Lib ${_Botan_shared_lib}")

# Aggiungi una libreria INTERFACE per OpenCL
add_library(OpenCL INTERFACE)

# Specifica dove trovare header e libreria per OpenCL
target_include_directories(OpenCL INTERFACE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(OpenCL INTERFACE ${CMAKE_SOURCE_DIR}/lib/OpenCL.lib)

# Aggiungi la directory del src (per lib)
add_subdirectory(src)

# Crea l'eseguibile
add_executable(AES_OpenCL src/main.cpp)

set_target_properties(AES_OpenCL PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
qt_standard_project_setup()

# Collega libreria
target_link_libraries(AES_OpenCL PRIVATE AES_OpenCL_lib)

# Imposta una variabile per il path alla DLL
if(NOT DEFINED ENV{Botan_LIB})
  message(FATAL_ERROR "La variabile d'ambiente 'Botan_LIB' non è definita.")
endif()

set(BOTAN_DLL_PATH "$ENV{Botan_LIB}/botan-3.dll")
string(REPLACE "\\" "/" BOTAN_DLL_PATH "${BOTAN_DLL_PATH}")

if(NOT EXISTS "${BOTAN_DLL_PATH}")
  message(FATAL_ERROR "File DLL non trovato: ${BOTAN_DLL_PATH}")
endif()

# Copia i kernels e il dll dopo la build
# La dll è necessaria sia nella cartella di build che in quella di test
add_custom_command(TARGET AES_OpenCL POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${BOTAN_DLL_PATH}"
            $<TARGET_FILE_DIR:AES_OpenCL>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${BOTAN_DLL_PATH}"
            "$<TARGET_FILE_DIR:AES_OpenCL>/tests"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/kernels
            $<TARGET_FILE_DIR:AES_OpenCL>/kernels
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/kernels
            "$<TARGET_FILE_DIR:AES_OpenCL>/tests/kernels"
)

# Abilita i test
enable_testing()

# Aggiungi la directory dei test
add_subdirectory(tests)

# Creo altri due eseguibili Bob e Alice
file(GLOB_RECURSE SOURCES_ALICE CONFIGURE_DEPENDS
    "src/Alice/*.cpp"
    "src/Alice/*.h"
    "src/Alice/*.ui"
)

file(GLOB_RECURSE SOURCES_BOB CONFIGURE_DEPENDS
    "src/Bob/*.cpp"
    "src/Bob/*.h"
    "src/Bob/*.ui"
)

add_executable(Alice ${SOURCES_ALICE})
add_executable(Bob ${SOURCES_BOB})

set_target_properties(Alice PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
target_link_libraries(Alice PRIVATE AES_OpenCL_lib)

set_target_properties(Bob PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)
target_link_libraries(Bob PRIVATE AES_OpenCL_lib)

# Installa gli eseguibili
install(TARGETS AES_OpenCL Alice Bob
    RUNTIME DESTINATION bin
)

# Installa i kernels
install(DIRECTORY "${CMAKE_SOURCE_DIR}/kernels"
        DESTINATION bin
)

# Installa la DLL di Botan
install(FILES "${BOTAN_DLL_PATH}"
        DESTINATION bin
)

# Installa DLL Qt
install(CODE "
  execute_process(COMMAND windeployqt \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/AES_OpenCL.exe)
  execute_process(COMMAND windeployqt \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Alice.exe)
  execute_process(COMMAND windeployqt \$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/Bob.exe)
")

# Messaggio finale
message(STATUS "AES_OpenCL configurato")
message(STATUS "Qt6 version: ${Qt6_VERSION}")
message(STATUS "Botan Version : ${Botan_VERSION}")