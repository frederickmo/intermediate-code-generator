# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/frederickmo/Documents/GitHub/intermediate-code-generator

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/frederickmo/Documents/GitHub/intermediate-code-generator/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/intermediate_code_generator.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/intermediate_code_generator.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/intermediate_code_generator.dir/flags.make

CMakeFiles/intermediate_code_generator.dir/main.cpp.o: CMakeFiles/intermediate_code_generator.dir/flags.make
CMakeFiles/intermediate_code_generator.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/frederickmo/Documents/GitHub/intermediate-code-generator/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/intermediate_code_generator.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/intermediate_code_generator.dir/main.cpp.o -c /Users/frederickmo/Documents/GitHub/intermediate-code-generator/main.cpp

CMakeFiles/intermediate_code_generator.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/intermediate_code_generator.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/frederickmo/Documents/GitHub/intermediate-code-generator/main.cpp > CMakeFiles/intermediate_code_generator.dir/main.cpp.i

CMakeFiles/intermediate_code_generator.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/intermediate_code_generator.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/frederickmo/Documents/GitHub/intermediate-code-generator/main.cpp -o CMakeFiles/intermediate_code_generator.dir/main.cpp.s

# Object files for target intermediate_code_generator
intermediate_code_generator_OBJECTS = \
"CMakeFiles/intermediate_code_generator.dir/main.cpp.o"

# External object files for target intermediate_code_generator
intermediate_code_generator_EXTERNAL_OBJECTS =

intermediate_code_generator: CMakeFiles/intermediate_code_generator.dir/main.cpp.o
intermediate_code_generator: CMakeFiles/intermediate_code_generator.dir/build.make
intermediate_code_generator: CMakeFiles/intermediate_code_generator.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/frederickmo/Documents/GitHub/intermediate-code-generator/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable intermediate_code_generator"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/intermediate_code_generator.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/intermediate_code_generator.dir/build: intermediate_code_generator
.PHONY : CMakeFiles/intermediate_code_generator.dir/build

CMakeFiles/intermediate_code_generator.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/intermediate_code_generator.dir/cmake_clean.cmake
.PHONY : CMakeFiles/intermediate_code_generator.dir/clean

CMakeFiles/intermediate_code_generator.dir/depend:
	cd /Users/frederickmo/Documents/GitHub/intermediate-code-generator/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/frederickmo/Documents/GitHub/intermediate-code-generator /Users/frederickmo/Documents/GitHub/intermediate-code-generator /Users/frederickmo/Documents/GitHub/intermediate-code-generator/cmake-build-debug /Users/frederickmo/Documents/GitHub/intermediate-code-generator/cmake-build-debug /Users/frederickmo/Documents/GitHub/intermediate-code-generator/cmake-build-debug/CMakeFiles/intermediate_code_generator.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/intermediate_code_generator.dir/depend
