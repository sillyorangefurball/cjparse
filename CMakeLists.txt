cmake_minimum_required(VERSION 3.18)

project(CJParse
		VERSION 0.1.1
		DESCRIPTION "JSON parser written in C"
		LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

add_library(cjparse STATIC src/cjparse.c src/io.c src/populate.c src/valparse.c)
target_include_directories(cjparse PUBLIC include)
target_link_libraries(cjparse PRIVATE m)
