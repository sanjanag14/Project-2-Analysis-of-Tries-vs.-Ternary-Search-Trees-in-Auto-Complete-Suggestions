cmake_minimum_required(VERSION 3.16)

project(autocomplete_structures CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# add tst library
add_library(tst STATIC
    src/tst.cpp
)

target_include_directories(tst PUBLIC include)

# build the main executable
add_executable(autocomplete_app
    src/main.cpp
    src/trie.cpp
)

target_include_directories(autocomplete_app PRIVATE include)
target_link_libraries(autocomplete_app PRIVATE tst)
