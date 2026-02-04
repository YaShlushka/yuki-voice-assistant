cmake_minimum_required(VERSION 3.14)
project(yuki-voice-assistant C CXX)

set(CMAKE_CXX_STANDARD 20)
set(BOOST_INCLUDE_LIBRARIES json locale)

add_subdirectory(lib/rapidfuzz)
add_subdirectory(lib/boost EXCLUDE_FROM_ALL)

find_package(Threads REQUIRED)
find_library(vosk_lib 
    NAMES vosk 
    PATHS "${CMAKE_CURRENT_SOURCE_DIR}/dynamic-lib"
    NO_DEFAULT_PATH
)

set(SOURCE_FILES
	src/main.cpp
	src/recognize-model.cpp
	src/voice-assistant.cpp
	src/common.cpp
	lib/miniaudio/miniaudio.c
	src/context-graph.h
	src/context-graph.cpp
	src/request.h
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE 
	"lib"
	"lib/vosk-api/src"
)

target_link_libraries(${PROJECT_NAME} PRIVATE
	${vosk_lib}
	rapidfuzz::rapidfuzz
	Boost::json
	Boost::locale
	Threads::Threads
)

if(UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE dl m)
endif()
