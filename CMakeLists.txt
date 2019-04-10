cmake_minimum_required(VERSION 3.12)
project(data_structure_for_love)

set(CMAKE_CXX_STANDARD 17)
LINK_LIBRARIES(gcov)
ADD_COMPILE_OPTIONS(-O0 -fprofile-arcs -ftest-coverage)
include(misc/test_helper.cmake)
#include(misc/library_helper.cmake)
include_directories(utils src/array_like src/memory src/binary_tree src/list_based src/heap)


#add_library(array_like STATIC src/array_like/optimized_vectorsxs.hpp src/array_like/array_queue.hpp)
#SET_TARGET_PROPERTIES(array_like PROPERTIES LINKER_LANGUAGE CXX)

unit_test(xor_list)
unit_test(binary_tree_base)
unit_test(optimized_vector)
unit_test(array_queue)
unit_test(object_pool)
#unit_test(cheney_heap)
unit_test(binary_heap)
unit_test(binomial_heap)
enable_testing()
