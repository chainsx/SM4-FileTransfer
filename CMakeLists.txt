cmake_minimum_required(VERSION 3.22)
project(FileTransfer C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_executable(send_file send_file.c
        src/zip.c
        )
add_executable(receive_file receive_file.c)

add_executable(sm4 sm4.c
        src/sm4_common.c
        src/sm4_enc.c
        src/sm4_modes.c
        src/sm4_setkey.c
        src/gcm.c
        src/block_cipher.c
        src/aes.c
        src/aes_modes.c
        src/gf128.c
        src/hex.c
        )
