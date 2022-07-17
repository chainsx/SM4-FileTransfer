cmake_minimum_required(VERSION 3.22)
project(FileTransfer C)

#set(CMAKE_MACOSX_RPATH 1)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_library(filetransfer
        src/messages.c
        src/connect.c
        src/get.c
        src/send.c
        src/sm4_file.c
        src/file_utils.c
        src/miniz/zip.c
        src/gmssl/sm4_common.c
        src/gmssl/sm4_enc.c
        src/gmssl/sm4_modes.c
        src/gmssl/sm4_setkey.c
        src/gmssl/gcm.c
        src/gmssl/block_cipher.c
        src/gmssl/aes.c
        src/gmssl/aes_modes.c
        src/gmssl/gf128.c
        src/gmssl/hex.c
        )

add_executable(filetransfer-bin
        tools/filetransfer.c
        )

target_link_libraries (filetransfer-bin LINK_PUBLIC filetransfer)
set_target_properties (filetransfer-bin PROPERTIES RUNTIME_OUTPUT_NAME filetransfer)

INSTALL(TARGETS filetransfer ARCHIVE DESTINATION lib  LIBRARY DESTINATION lib)
INSTALL(DIRECTORY ${CMAKE_SOURCE_DIR}/include/filetransfer DESTINATION include)
INSTALL(TARGETS filetransfer-bin RUNTIME DESTINATION bin)
