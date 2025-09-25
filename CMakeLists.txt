cmake_minimum_required(VERSION 3.14)
project(yuki-voice-assistant CXX)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_SYSTEM_NAME MATCHES "^MINGW")
	set(SYSTEM_LIBS -lstdc++)
else()
	set(SYSTEM_LIBS)
endif()

set(SOURCE_FILES
    main.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${SYSTEM_LIBS})
