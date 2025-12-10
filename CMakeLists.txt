cmake_minimum_required(VERSION 3.14)
project(yuki-voice-assistant C CXX)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES
	src/main.cpp src/recognize_model.cpp lib/miniaudio/miniaudio.c lib/json/json.cpp
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(context_graph_lib STATIC src/context_graph.h src/context_graph.cpp src/request.h)
add_subdirectory(lib/whisper.cpp)
add_subdirectory(lib/catch2)
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

add_executable(recognize_test
	tests/recognize_model_tests/vosk_api_recognize_model.cpp
	tests/recognize_model_tests/vosk_api_recognize_model.h
	tests/recognize_model_tests/vosk_wav_recognize.h
	tests/recognize_model_tests/whisper_recognize_model.cpp
	tests/recognize_model_tests/whisper_recognize_model.h
	tests/recognize_model_tests/whisper_wav_recognize.h
	tests/recognize_model_tests/main.cpp
	lib/json/json.cpp
)

add_executable(unit_tests
	tests/unit_tests.cpp
)

target_include_directories(recognize_test PRIVATE "lib")
target_include_directories(unit_tests PRIVATE "lib/catch2/include")

target_link_libraries(recognize_test vosk whisper)
target_link_libraries(unit_tests Catch2::Catch2WithMain)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SYSTEM_LIBS} whisper)
