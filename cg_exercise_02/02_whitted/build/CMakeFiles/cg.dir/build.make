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
CMAKE_SOURCE_DIR = /media/sf_shared/cg_exercise_02/02_whitted

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /media/sf_shared/cg_exercise_02/02_whitted/build

# Include any dependencies generated for this target.
include CMakeFiles/cg.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cg.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cg.dir/flags.make

CMakeFiles/cg.dir/src/main.cpp.o: CMakeFiles/cg.dir/flags.make
CMakeFiles/cg.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_shared/cg_exercise_02/02_whitted/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/cg.dir/src/main.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cg.dir/src/main.cpp.o -c /media/sf_shared/cg_exercise_02/02_whitted/src/main.cpp

CMakeFiles/cg.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cg.dir/src/main.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/sf_shared/cg_exercise_02/02_whitted/src/main.cpp > CMakeFiles/cg.dir/src/main.cpp.i

CMakeFiles/cg.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cg.dir/src/main.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/sf_shared/cg_exercise_02/02_whitted/src/main.cpp -o CMakeFiles/cg.dir/src/main.cpp.s

CMakeFiles/cg.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/cg.dir/src/main.cpp.o.requires

CMakeFiles/cg.dir/src/main.cpp.o.provides: CMakeFiles/cg.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/cg.dir/build.make CMakeFiles/cg.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/cg.dir/src/main.cpp.o.provides

CMakeFiles/cg.dir/src/main.cpp.o.provides.build: CMakeFiles/cg.dir/src/main.cpp.o


CMakeFiles/cg.dir/src/exercise_02.cpp.o: CMakeFiles/cg.dir/flags.make
CMakeFiles/cg.dir/src/exercise_02.cpp.o: ../src/exercise_02.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/media/sf_shared/cg_exercise_02/02_whitted/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/cg.dir/src/exercise_02.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/cg.dir/src/exercise_02.cpp.o -c /media/sf_shared/cg_exercise_02/02_whitted/src/exercise_02.cpp

CMakeFiles/cg.dir/src/exercise_02.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/cg.dir/src/exercise_02.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /media/sf_shared/cg_exercise_02/02_whitted/src/exercise_02.cpp > CMakeFiles/cg.dir/src/exercise_02.cpp.i

CMakeFiles/cg.dir/src/exercise_02.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/cg.dir/src/exercise_02.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /media/sf_shared/cg_exercise_02/02_whitted/src/exercise_02.cpp -o CMakeFiles/cg.dir/src/exercise_02.cpp.s

CMakeFiles/cg.dir/src/exercise_02.cpp.o.requires:

.PHONY : CMakeFiles/cg.dir/src/exercise_02.cpp.o.requires

CMakeFiles/cg.dir/src/exercise_02.cpp.o.provides: CMakeFiles/cg.dir/src/exercise_02.cpp.o.requires
	$(MAKE) -f CMakeFiles/cg.dir/build.make CMakeFiles/cg.dir/src/exercise_02.cpp.o.provides.build
.PHONY : CMakeFiles/cg.dir/src/exercise_02.cpp.o.provides

CMakeFiles/cg.dir/src/exercise_02.cpp.o.provides.build: CMakeFiles/cg.dir/src/exercise_02.cpp.o


# Object files for target cg
cg_OBJECTS = \
"CMakeFiles/cg.dir/src/main.cpp.o" \
"CMakeFiles/cg.dir/src/exercise_02.cpp.o"

# External object files for target cg
cg_EXTERNAL_OBJECTS =

../cg: CMakeFiles/cg.dir/src/main.cpp.o
../cg: CMakeFiles/cg.dir/src/exercise_02.cpp.o
../cg: CMakeFiles/cg.dir/build.make
../cg: /media/sf_shared/cg_exercise_02/cglib/build/cglib/libcglib_cg_2.a
../cg: /usr/lib/i386-linux-gnu/libGLEW.so
../cg: /usr/lib/i386-linux-gnu/libglfw.so
../cg: /usr/lib/i386-linux-gnu/libGLU.so
../cg: /usr/lib/i386-linux-gnu/libGL.so
../cg: CMakeFiles/cg.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/media/sf_shared/cg_exercise_02/02_whitted/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ../cg"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cg.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cg.dir/build: ../cg

.PHONY : CMakeFiles/cg.dir/build

CMakeFiles/cg.dir/requires: CMakeFiles/cg.dir/src/main.cpp.o.requires
CMakeFiles/cg.dir/requires: CMakeFiles/cg.dir/src/exercise_02.cpp.o.requires

.PHONY : CMakeFiles/cg.dir/requires

CMakeFiles/cg.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cg.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cg.dir/clean

CMakeFiles/cg.dir/depend:
	cd /media/sf_shared/cg_exercise_02/02_whitted/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /media/sf_shared/cg_exercise_02/02_whitted /media/sf_shared/cg_exercise_02/02_whitted /media/sf_shared/cg_exercise_02/02_whitted/build /media/sf_shared/cg_exercise_02/02_whitted/build /media/sf_shared/cg_exercise_02/02_whitted/build/CMakeFiles/cg.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cg.dir/depend

