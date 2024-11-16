cmake_minimum_required(VERSION 3.17)
project(Huffman)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(Huffman main.cpp Bit.cpp Bit.h your_huffman_code.h)