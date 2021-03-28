cmake_minimum_required(VERSION 3.5)

project(BigNumber LANGUAGES CXX)
find_package (Threads)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(BigNumber main.cpp BigInt.hpp Operators.hpp Converter.hpp )
target_link_libraries (BigNumber ${CMAKE_THREAD_LIBS_INIT})
