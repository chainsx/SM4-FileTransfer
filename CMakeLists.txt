cmake_minimum_required(VERSION 3.22)
project(FileTransfer C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_executable(sm4 sm4test.c
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

add_executable(server server.c
        src/mmapFile.c
        src/threadpool.c
        src/transfer.c
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
        src/gmssl/hex.c)
add_executable(client client.c
        src/mmapFile.c
        src/threadpool.c
        src/transfer.c
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
        src/gmssl/hex.c)
