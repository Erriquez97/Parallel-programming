# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/domenico/VideoMotionDetect

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/domenico/VideoMotionDetect/build

# Include any dependencies generated for this target.
include CMakeFiles/Prova.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Prova.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Prova.dir/flags.make

CMakeFiles/Prova.dir/prova.cpp.o: CMakeFiles/Prova.dir/flags.make
CMakeFiles/Prova.dir/prova.cpp.o: ../prova.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/domenico/VideoMotionDetect/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Prova.dir/prova.cpp.o"
	/usr/bin/g++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Prova.dir/prova.cpp.o -c /home/domenico/VideoMotionDetect/prova.cpp

CMakeFiles/Prova.dir/prova.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Prova.dir/prova.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/domenico/VideoMotionDetect/prova.cpp > CMakeFiles/Prova.dir/prova.cpp.i

CMakeFiles/Prova.dir/prova.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Prova.dir/prova.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/domenico/VideoMotionDetect/prova.cpp -o CMakeFiles/Prova.dir/prova.cpp.s

# Object files for target Prova
Prova_OBJECTS = \
"CMakeFiles/Prova.dir/prova.cpp.o"

# External object files for target Prova
Prova_EXTERNAL_OBJECTS =

Prova: CMakeFiles/Prova.dir/prova.cpp.o
Prova: CMakeFiles/Prova.dir/build.make
Prova: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_aruco.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_bgsegm.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_bioinspired.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_ccalib.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_dnn_objdetect.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_dnn_superres.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_dpm.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_face.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_freetype.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_fuzzy.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_hdf.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_hfs.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_img_hash.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_line_descriptor.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_quality.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_reg.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_rgbd.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_saliency.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_shape.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_stereo.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_structured_light.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_surface_matching.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_tracking.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_viz.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_xobjdetect.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_xphoto.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_datasets.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_plot.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_text.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_dnn.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_phase_unwrapping.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_optflow.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_ximgproc.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_video.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_videoio.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.4.2.0
Prova: /usr/lib/x86_64-linux-gnu/libopencv_core.so.4.2.0
Prova: CMakeFiles/Prova.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/domenico/VideoMotionDetect/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Prova"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Prova.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Prova.dir/build: Prova

.PHONY : CMakeFiles/Prova.dir/build

CMakeFiles/Prova.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Prova.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Prova.dir/clean

CMakeFiles/Prova.dir/depend:
	cd /home/domenico/VideoMotionDetect/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/domenico/VideoMotionDetect /home/domenico/VideoMotionDetect /home/domenico/VideoMotionDetect/build /home/domenico/VideoMotionDetect/build /home/domenico/VideoMotionDetect/build/CMakeFiles/Prova.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Prova.dir/depend

