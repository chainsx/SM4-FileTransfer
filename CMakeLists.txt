cmake_minimum_required(VERSION 3.22)
project(FileTransfer C)

set(CMAKE_C_STANDARD 99)

add_executable(send_file send_file.c)
add_executable(receive_file receive_file.c)
