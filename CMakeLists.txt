cmake_minimum_required(VERSION 3.14)
project(yuki-voice-assistant C CXX)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
	src/main.cpp src/recognize_model.cpp lib/miniaudio/miniaudio.c lib/json/json.cpp
)

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(lib/whisper.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE "lib")

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_libraries(${PROJECT_NAME} PRIVATE dl pthread m)
endif()

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
	set(SYSTEM_LIBS -lstdc++)
else()
	set(SYSTEM_LIBS)
endif()

add_executable(vosk_recognize_test
	tests/recognize_model_tests/vosk_api_recognize_model.cpp
	tests/recognize_model_tests/vosk_api_recognize_model.h
	tests/recognize_model_tests/vosk_wav_recognize.h
	tests/recognize_model_tests/main.cpp
	lib/json/json.cpp
)

target_include_directories(vosk_recognize_test PRIVATE "lib")

target_link_libraries(vosk_recognize_test vosk)

target_link_libraries(${PROJECT_NAME} PRIVATE ${SYSTEM_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE whisper)
