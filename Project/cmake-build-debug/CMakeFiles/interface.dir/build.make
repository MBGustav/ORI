# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.29

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

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2024.1\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2024.1\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\enzod\Desktop\ORI\Project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\enzod\Desktop\ORI\Project\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/interface.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/interface.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/interface.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/interface.dir/flags.make

CMakeFiles/interface.dir/cli-terface.cpp.obj: CMakeFiles/interface.dir/flags.make
CMakeFiles/interface.dir/cli-terface.cpp.obj: CMakeFiles/interface.dir/includes_CXX.rsp
CMakeFiles/interface.dir/cli-terface.cpp.obj: C:/Users/enzod/Desktop/ORI/Project/cli-terface.cpp
CMakeFiles/interface.dir/cli-terface.cpp.obj: CMakeFiles/interface.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=C:\Users\enzod\Desktop\ORI\Project\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/interface.dir/cli-terface.cpp.obj"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/interface.dir/cli-terface.cpp.obj -MF CMakeFiles\interface.dir\cli-terface.cpp.obj.d -o CMakeFiles\interface.dir\cli-terface.cpp.obj -c C:\Users\enzod\Desktop\ORI\Project\cli-terface.cpp

CMakeFiles/interface.dir/cli-terface.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/interface.dir/cli-terface.cpp.i"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\enzod\Desktop\ORI\Project\cli-terface.cpp > CMakeFiles\interface.dir\cli-terface.cpp.i

CMakeFiles/interface.dir/cli-terface.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/interface.dir/cli-terface.cpp.s"
	C:\PROGRA~1\JETBRA~1\CLION2~1.1\bin\mingw\bin\G__~1.EXE $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\enzod\Desktop\ORI\Project\cli-terface.cpp -o CMakeFiles\interface.dir\cli-terface.cpp.s

# Object files for target interface
interface_OBJECTS = \
"CMakeFiles/interface.dir/cli-terface.cpp.obj"

# External object files for target interface
interface_EXTERNAL_OBJECTS =

C:/Users/enzod/Desktop/ORI/Project/interface.exe: CMakeFiles/interface.dir/cli-terface.cpp.obj
C:/Users/enzod/Desktop/ORI/Project/interface.exe: CMakeFiles/interface.dir/build.make
C:/Users/enzod/Desktop/ORI/Project/interface.exe: libScratchQL_lib.a
C:/Users/enzod/Desktop/ORI/Project/interface.exe: CMakeFiles/interface.dir/linkLibs.rsp
C:/Users/enzod/Desktop/ORI/Project/interface.exe: CMakeFiles/interface.dir/objects1.rsp
C:/Users/enzod/Desktop/ORI/Project/interface.exe: CMakeFiles/interface.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=C:\Users\enzod\Desktop\ORI\Project\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable C:\Users\enzod\Desktop\ORI\Project\interface.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\interface.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/interface.dir/build: C:/Users/enzod/Desktop/ORI/Project/interface.exe
.PHONY : CMakeFiles/interface.dir/build

CMakeFiles/interface.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\interface.dir\cmake_clean.cmake
.PHONY : CMakeFiles/interface.dir/clean

CMakeFiles/interface.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\enzod\Desktop\ORI\Project C:\Users\enzod\Desktop\ORI\Project C:\Users\enzod\Desktop\ORI\Project\cmake-build-debug C:\Users\enzod\Desktop\ORI\Project\cmake-build-debug C:\Users\enzod\Desktop\ORI\Project\cmake-build-debug\CMakeFiles\interface.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/interface.dir/depend

