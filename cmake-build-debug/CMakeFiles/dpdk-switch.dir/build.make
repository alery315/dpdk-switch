# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /home/alery/clion-2019.2.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/alery/clion-2019.2.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alery/CLionProjects/dpdk-switch

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alery/CLionProjects/dpdk-switch/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/dpdk-switch.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/dpdk-switch.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dpdk-switch.dir/flags.make

CMakeFiles/dpdk-switch.dir/main.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/main.c.o: ../main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/dpdk-switch.dir/main.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/main.c.o   -c /home/alery/CLionProjects/dpdk-switch/main.c

CMakeFiles/dpdk-switch.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/main.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/main.c > CMakeFiles/dpdk-switch.dir/main.c.i

CMakeFiles/dpdk-switch.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/main.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/main.c -o CMakeFiles/dpdk-switch.dir/main.c.s

CMakeFiles/dpdk-switch.dir/forwarding.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/forwarding.c.o: ../forwarding.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/dpdk-switch.dir/forwarding.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/forwarding.c.o   -c /home/alery/CLionProjects/dpdk-switch/forwarding.c

CMakeFiles/dpdk-switch.dir/forwarding.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/forwarding.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/forwarding.c > CMakeFiles/dpdk-switch.dir/forwarding.c.i

CMakeFiles/dpdk-switch.dir/forwarding.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/forwarding.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/forwarding.c -o CMakeFiles/dpdk-switch.dir/forwarding.c.s

CMakeFiles/dpdk-switch.dir/receive.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/receive.c.o: ../receive.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/dpdk-switch.dir/receive.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/receive.c.o   -c /home/alery/CLionProjects/dpdk-switch/receive.c

CMakeFiles/dpdk-switch.dir/receive.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/receive.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/receive.c > CMakeFiles/dpdk-switch.dir/receive.c.i

CMakeFiles/dpdk-switch.dir/receive.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/receive.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/receive.c -o CMakeFiles/dpdk-switch.dir/receive.c.s

CMakeFiles/dpdk-switch.dir/transmit.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/transmit.c.o: ../transmit.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building C object CMakeFiles/dpdk-switch.dir/transmit.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/transmit.c.o   -c /home/alery/CLionProjects/dpdk-switch/transmit.c

CMakeFiles/dpdk-switch.dir/transmit.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/transmit.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/transmit.c > CMakeFiles/dpdk-switch.dir/transmit.c.i

CMakeFiles/dpdk-switch.dir/transmit.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/transmit.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/transmit.c -o CMakeFiles/dpdk-switch.dir/transmit.c.s

CMakeFiles/dpdk-switch.dir/init.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/init.c.o: ../init.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building C object CMakeFiles/dpdk-switch.dir/init.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/init.c.o   -c /home/alery/CLionProjects/dpdk-switch/init.c

CMakeFiles/dpdk-switch.dir/init.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/init.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/init.c > CMakeFiles/dpdk-switch.dir/init.c.i

CMakeFiles/dpdk-switch.dir/init.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/init.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/init.c -o CMakeFiles/dpdk-switch.dir/init.c.s

CMakeFiles/dpdk-switch.dir/config.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/config.c.o: ../config.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building C object CMakeFiles/dpdk-switch.dir/config.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/config.c.o   -c /home/alery/CLionProjects/dpdk-switch/config.c

CMakeFiles/dpdk-switch.dir/config.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/config.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/config.c > CMakeFiles/dpdk-switch.dir/config.c.i

CMakeFiles/dpdk-switch.dir/config.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/config.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/config.c -o CMakeFiles/dpdk-switch.dir/config.c.s

CMakeFiles/dpdk-switch.dir/output_queue.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/output_queue.c.o: ../output_queue.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building C object CMakeFiles/dpdk-switch.dir/output_queue.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/output_queue.c.o   -c /home/alery/CLionProjects/dpdk-switch/output_queue.c

CMakeFiles/dpdk-switch.dir/output_queue.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/output_queue.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/output_queue.c > CMakeFiles/dpdk-switch.dir/output_queue.c.i

CMakeFiles/dpdk-switch.dir/output_queue.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/output_queue.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/output_queue.c -o CMakeFiles/dpdk-switch.dir/output_queue.c.s

CMakeFiles/dpdk-switch.dir/test.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/test.c.o: ../test.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building C object CMakeFiles/dpdk-switch.dir/test.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/test.c.o   -c /home/alery/CLionProjects/dpdk-switch/test.c

CMakeFiles/dpdk-switch.dir/test.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/test.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/test.c > CMakeFiles/dpdk-switch.dir/test.c.i

CMakeFiles/dpdk-switch.dir/test.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/test.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/test.c -o CMakeFiles/dpdk-switch.dir/test.c.s

CMakeFiles/dpdk-switch.dir/log.c.o: CMakeFiles/dpdk-switch.dir/flags.make
CMakeFiles/dpdk-switch.dir/log.c.o: ../log.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building C object CMakeFiles/dpdk-switch.dir/log.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/dpdk-switch.dir/log.c.o   -c /home/alery/CLionProjects/dpdk-switch/log.c

CMakeFiles/dpdk-switch.dir/log.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dpdk-switch.dir/log.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/alery/CLionProjects/dpdk-switch/log.c > CMakeFiles/dpdk-switch.dir/log.c.i

CMakeFiles/dpdk-switch.dir/log.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dpdk-switch.dir/log.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/alery/CLionProjects/dpdk-switch/log.c -o CMakeFiles/dpdk-switch.dir/log.c.s

# Object files for target dpdk-switch
dpdk__switch_OBJECTS = \
"CMakeFiles/dpdk-switch.dir/main.c.o" \
"CMakeFiles/dpdk-switch.dir/forwarding.c.o" \
"CMakeFiles/dpdk-switch.dir/receive.c.o" \
"CMakeFiles/dpdk-switch.dir/transmit.c.o" \
"CMakeFiles/dpdk-switch.dir/init.c.o" \
"CMakeFiles/dpdk-switch.dir/config.c.o" \
"CMakeFiles/dpdk-switch.dir/output_queue.c.o" \
"CMakeFiles/dpdk-switch.dir/test.c.o" \
"CMakeFiles/dpdk-switch.dir/log.c.o"

# External object files for target dpdk-switch
dpdk__switch_EXTERNAL_OBJECTS =

dpdk-switch: CMakeFiles/dpdk-switch.dir/main.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/forwarding.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/receive.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/transmit.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/init.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/config.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/output_queue.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/test.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/log.c.o
dpdk-switch: CMakeFiles/dpdk-switch.dir/build.make
dpdk-switch: CMakeFiles/dpdk-switch.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking C executable dpdk-switch"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dpdk-switch.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dpdk-switch.dir/build: dpdk-switch

.PHONY : CMakeFiles/dpdk-switch.dir/build

CMakeFiles/dpdk-switch.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dpdk-switch.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dpdk-switch.dir/clean

CMakeFiles/dpdk-switch.dir/depend:
	cd /home/alery/CLionProjects/dpdk-switch/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alery/CLionProjects/dpdk-switch /home/alery/CLionProjects/dpdk-switch /home/alery/CLionProjects/dpdk-switch/cmake-build-debug /home/alery/CLionProjects/dpdk-switch/cmake-build-debug /home/alery/CLionProjects/dpdk-switch/cmake-build-debug/CMakeFiles/dpdk-switch.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dpdk-switch.dir/depend

