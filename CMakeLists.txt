cmake_minimum_required(VERSION 3.27)
project(uloha2)

set(CMAKE_CXX_STANDARD 20)

add_executable(uloha2
        main.cpp)
if(APPLE)
    target_compile_options(uloha2 PRIVATE -stdlib=libc++)
    target_link_libraries(uloha2 PRIVATE c++)
endif()