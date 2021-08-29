cmake_minimum_required(VERSION 3.13)
project(StereoPiMap3D)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(mjpeg_http "")
add_custom_command(TARGET mjpeg_http POST_BUILD
	COMMAND cp -r ${CMAKE_SOURCE_DIR}/mjpeg-http/html ${CMAKE_BINARY_DIR}
)
add_subdirectory(mjpeg-http)

add_executable(StereoPiMap3D "")
target_include_directories(StereoPiMap3D PRIVATE include)
find_package( OpenCV REQUIRED )
target_link_libraries(StereoPiMap3D PRIVATE 
	pthread
	mjpeg_http
	${OpenCV_LIBS}
)
add_subdirectory(src)