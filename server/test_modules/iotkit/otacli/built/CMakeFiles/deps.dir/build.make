# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built

# Utility rule file for deps.

# Include the progress variables for this target.
include CMakeFiles/deps.dir/progress.make

CMakeFiles/deps:
	cd /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli && ./gradlew --daemon dependencies --configuration compile

deps: CMakeFiles/deps
deps: CMakeFiles/deps.dir/build.make

.PHONY : deps

# Rule to build all files generated by this target.
CMakeFiles/deps.dir/build: deps

.PHONY : CMakeFiles/deps.dir/build

CMakeFiles/deps.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/deps.dir/cmake_clean.cmake
.PHONY : CMakeFiles/deps.dir/clean

CMakeFiles/deps.dir/depend:
	cd /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built /home/renxl/work/share/github/developing/server/test_modules/iotkit/otacli/built/CMakeFiles/deps.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/deps.dir/depend
