# Colleziona i sorgenti
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.c" "*.h" "*.hpp"
) 

list(REMOVE_ITEM SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"           # escludi main comune
    "${CMAKE_CURRENT_SOURCE_DIR}/Alice/alice_main.cpp"  # escludi main Alice
    "${CMAKE_CURRENT_SOURCE_DIR}/Bob/bob_main.cpp"      # escludi main Bob
)

add_library(AES_OpenCL_lib STATIC ${SOURCES})

target_include_directories(AES_OpenCL_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


# Per eseguire QT è necessario avere nel path il /bin
target_link_libraries(AES_OpenCL_lib
    PUBLIC
    Botan::Botan
    OpenCL
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

set_target_properties(AES_OpenCL_lib PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)