# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /nix/store/a4icwnjm9iasay87ggkfp4gw36wsqcpw-cmake-3.13.4/bin/cmake

# The command to remove a file.
RM = /nix/store/a4icwnjm9iasay87ggkfp4gw36wsqcpw-cmake-3.13.4/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/schrodinger/CLionProjects/data_structure_for_love

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/schrodinger/CLionProjects/data_structure_for_love/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/test_array_queue.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_array_queue.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_array_queue.dir/flags.make

CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.o: CMakeFiles/test_array_queue.dir/flags.make
CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.o: ../misc/test/test_array_queue.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/schrodinger/CLionProjects/data_structure_for_love/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.o"
	/run/current-system/sw/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.o -c /home/schrodinger/CLionProjects/data_structure_for_love/misc/test/test_array_queue.cpp

CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.i"
	/run/current-system/sw/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/schrodinger/CLionProjects/data_structure_for_love/misc/test/test_array_queue.cpp > CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.i

CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.s"
	/run/current-system/sw/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/schrodinger/CLionProjects/data_structure_for_love/misc/test/test_array_queue.cpp -o CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.s

# Object files for target test_array_queue
test_array_queue_OBJECTS = \
"CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.o"

# External object files for target test_array_queue
test_array_queue_EXTERNAL_OBJECTS =

test_array_queue: CMakeFiles/test_array_queue.dir/misc/test/test_array_queue.cpp.o
test_array_queue: CMakeFiles/test_array_queue.dir/build.make
test_array_queue: CMakeFiles/test_array_queue.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/schrodinger/CLionProjects/data_structure_for_love/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_array_queue"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_array_queue.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_array_queue.dir/build: test_array_queue

.PHONY : CMakeFiles/test_array_queue.dir/build

CMakeFiles/test_array_queue.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_array_queue.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_array_queue.dir/clean

CMakeFiles/test_array_queue.dir/depend:
	cd /home/schrodinger/CLionProjects/data_structure_for_love/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/schrodinger/CLionProjects/data_structure_for_love /home/schrodinger/CLionProjects/data_structure_for_love /home/schrodinger/CLionProjects/data_structure_for_love/cmake-build-debug /home/schrodinger/CLionProjects/data_structure_for_love/cmake-build-debug /home/schrodinger/CLionProjects/data_structure_for_love/cmake-build-debug/CMakeFiles/test_array_queue.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_array_queue.dir/depend

