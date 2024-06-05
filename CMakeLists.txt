cmake_minimum_required(VERSION 3.5)

project(tspectrogram)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_STANDARD 20)
add_compile_options(-Wall -Wextra -Wpedantic)
add_compile_options(-g3)

add_executable(tspectrogram)
target_sources(tspectrogram PRIVATE
	src/main.cpp
)
target_include_directories(tspectrogram PUBLIC src)

add_subdirectory(lib)
target_link_libraries(tspectrogram portaudio)
