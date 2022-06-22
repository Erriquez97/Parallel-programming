cmake_minimum_required(VERSION 2.8)
project( VideoMotionDetect )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable( VideoMotionDetect VideoMotionDetect.cpp )
target_link_libraries( VideoMotionDetect ${OpenCV_LIBS} )
target_include_directories( VideoMotionDetect PUBLIC "./include/")
# add_compile_options(-O3)
# add_compile_options(-ftree-vectorize)
# add_compile_options(-fopt-info-vec-all)


SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -ftree-vectorize")