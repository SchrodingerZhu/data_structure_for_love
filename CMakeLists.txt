cmake_minimum_required(VERSION 3.12)
project(data_structure_for_love)

set(CMAKE_CXX_STANDARD 17)
add_compile_options(-O2)
include(misc/test_helper.cmake)
#include(misc/library_helper.cmake)
include_directories(utils src/array_like src/memory)

#add_library(array_like STATIC src/array_like/optimized_vectorsxs.hpp src/array_like/array_queue.hpp)
#SET_TARGET_PROPERTIES(array_like PROPERTIES LINKER_LANGUAGE CXX)


unit_test(optimized_vector)
unit_test(array_queue)
unit_test(object_pool)
enable_testing()
