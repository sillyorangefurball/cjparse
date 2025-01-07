cmake_minimum_required(VERSION 3.18)

project(cjparse
		VERSION 0.1.1
		DESCRIPTION "JSON parser written in C"
		LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CJPARSE_INCLUDE_DIRS "include")

file(GLOB_RECURSE CJPARSE_SRC "src/*.c")
file(GLOB_RECURSE CJPARSE_PUB_INCLUDE "include/*.h")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
	message(STATUS "CMake build type not set, defaulting to Release")
endif()

add_library(${PROJECT_NAME} ${CJPARSE_SRC})
target_include_directories(${PROJECT_NAME} PRIVATE ${CJPARSE_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE m)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	PUBLIC_HEADER "${CJPARSE_PUB_INCLUDE}"
)

install(TARGETS ${PROJECT_NAME}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	PUBLIC_HEADER DESTINATION include/${PROJECT_NAME}
)
