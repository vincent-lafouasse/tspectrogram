cmake_minimum_required(VERSION 3.5)

project(tspectrogram)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 17)

add_executable(tspectrogram)
target_sources(tspectrogram PRIVATE
	src/main.cpp
	src/InputStream.cpp
)
set_target_properties(tspectrogram PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic -g3")
target_include_directories(tspectrogram PUBLIC src)

add_subdirectory(lib)
target_link_libraries(tspectrogram portaudio)
