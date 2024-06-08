cmake_minimum_required(VERSION 3.5)

project(tspectrogram)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_C_STANDARD 99)

add_executable(tspectrogram)
target_sources(tspectrogram PRIVATE
	src/main.c
	src/InputStream.c
	src/InputStream.h
	src/ui.c
	src/ui.h
)
set_target_properties(tspectrogram PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic -g3")
target_include_directories(tspectrogram PUBLIC src)

include(cmake/CPM.cmake)

CPMAddPackage(
    NAME portaudio
    GITHUB_REPOSITORY PortAudio/portaudio
    VERSION 19.7.0
)
target_include_directories(tspectrogram PUBLIC ${portaudio_SOURCE_DIR}/include)
target_link_libraries(tspectrogram portaudio)

CPMAddPackage(
    NAME fftw3
    URL "http://fftw.org/fftw-3.3.10.tar.gz"
    VERSION 3.3.10
)
target_include_directories(tspectrogram PUBLIC ${fftw3_SOURCE_DIR}/api)
target_link_libraries(tspectrogram fftw3)
