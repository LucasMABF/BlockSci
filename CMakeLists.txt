cmake_minimum_required(VERSION 3.10...3.31)
project(blocksci)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")

set(BlockSci_VERSION 0.7.0)

include(GNUInstallDirs)

option(BLOCKSCI_NATIVE_ARCH "Tune the build for the host CPU (-march=native; impure, non-portable)" OFF)

add_subdirectory(external)

add_subdirectory(src)
add_subdirectory(tools)
add_subdirectory(benchmark)
add_subdirectory(test)
add_subdirectory(example)
