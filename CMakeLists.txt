cmake_minimum_required(VERSION 3.14)
project(yuki-voice-assistant C CXX)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
	main.cpp miniaudio.c
)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "whisper.cpp/include")
target_include_directories(${PROJECT_NAME} PRIVATE "whisper.cpp/ggml/include")
target_link_libraries(${PROJECT_NAME} PRIVATE ${SYSTEM_LIBS})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} PRIVATE dl pthread m)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
	set(SYSTEM_LIBS -lstdc++)
else()
	set(SYSTEM_LIBS)
endif()
