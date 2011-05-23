# CMake build script for the git2 project
#
# Building (out of source build):
# > mkdir build && cd build
# > cmake .. [-DSETTINGS=VALUE]
# > cmake --build .
#
# Install:
# > cmake --build . --target install

project (Git2 C)
cmake_minimum_required (VERSION 2.6)

set(ROOT_DIRECTORY ${CMAKE_SOURCE_DIR}/..)
set(SRC_DIRECTORY ${ROOT_DIRECTORY}/src)
include_directories(
	${SRC_DIRECTORY}
	${SRC_DIRECTORY}/common
	${SRC_DIRECTORY}/common/utils
	${SRC_DIRECTORY}/common/commands
	${SRC_DIRECTORY}/builtin
	#place new include directories here
)
set(TESTS_DIRECTORY ${ROOT_DIRECTORY}/tests)
set(LIBGIT2_DIRECTORY ${ROOT_DIRECTORY}/libgit2)
set(LIBGIT2_BUILD_DIRECTORY ${LIBGIT2_DIRECTORY}/build)
set(BINARIES_DIRECTORY ${ROOT_DIRECTORY}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${BINARIES_DIRECTORY})
file(GLOB_RECURSE C_SOURCE_FILES ${SRC_DIRECTORY}/*.c)

set(CMAKE_C_FLAGS "-std=gnu99 -Wall -Wextra -pedantic")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 ${CMAKE_C_FLAGS}")
set(CMAKE_C_FLAGS_RELEASE "-O2 ${CMAKE_C_FLAGS}")

#include libssl
find_package(OpenSSL)
add_definitions(-DOPENSSL_SHA1)
include_directories(${OPENSSL_INCLUDE_DIR})
set(LIB_SHA1 ${OPENSSL_CRYPTO_LIBRARIES})


set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(${LIBGIT2_DIRECTORY}/include)
set(LIBS ${LIBS} git2.a ${LIB_SHA1})
link_directories(${LIBGIT2_BUILD_DIRECTORY})

add_custom_target(test COMMAND $(MAKE) all WORKING_DIRECTORY ${TESTS_DIRECTORY} DEPENDS git2-bin)
add_custom_target(
	build_libgit2
	COMMAND
		cmake -D BUILD_SHARED_LIBS:OPTION=OFF -D BUILD_TESTS:OPTION=OFF . &&
		cmake --build .
	DEPENDS "${LIBGIT2_BUILD_DIRECTORY}/Makefile"
	WORKING_DIRECTORY ${LIBGIT2_BUILD_DIRECTORY}
)

add_custom_command(
	OUTPUT "${LIBGIT2_BUILD_DIRECTORY}/Makefile"
	COMMAND
		mkdir -p ${LIBGIT2_BUILD_DIRECTORY} &&
		cd ${LIBGIT2_BUILD_DIRECTORY} &&
		cmake ${LIBGIT2_DIRECTORY}
)

add_executable(git2-bin ${C_SOURCE_FILES})
set_target_properties(git2-bin PROPERTIES OUTPUT_NAME git2)
add_dependencies(git2-bin build_libgit2)
target_link_libraries(git2-bin ${LIBS})
