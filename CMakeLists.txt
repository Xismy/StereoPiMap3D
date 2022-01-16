cmake_minimum_required(VERSION 3.13)
project(StereoPiMap3D)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(mjpeg_http "")
add_custom_command(TARGET mjpeg_http POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/mjpeg-http/html ${CMAKE_BINARY_DIR}/html
)

add_subdirectory(mjpeg-http)

add_executable(StereoPiMap3D "")
add_executable(StereoPiMap3DTests "")
add_custom_command(TARGET StereoPiMap3DTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/tests/xml ${CMAKE_BINARY_DIR}/xml
)

target_include_directories(StereoPiMap3D PRIVATE include)
target_include_directories(StereoPiMap3DTests PRIVATE include ${GTEST_INCLUDE_DIR})

find_package( OpenCV REQUIRED )
find_package(GTest REQUIRED) 

target_link_libraries(StereoPiMap3D PRIVATE 
	pthread
	mjpeg_http
	opencv_core
	opencv_calib3d
	opencv_videoio
)

target_link_libraries(StereoPiMap3DTests PRIVATE 
	pthread
	mjpeg_http
	opencv_core
	opencv_calib3d
	opencv_videoio
	${GTEST_BOTH_LIBRARIES}
)

add_subdirectory(src)
add_subdirectory(tests)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/tmp)