# CMake build script for the SimpleGit project
#
# Building (out of source build):
# > mkdir build && cd build
# > cmake .. [-DSETTINGS=VALUE]
# > cmake --build .
#
# Install:
# > cmake --build . --target install

SET(CMAKE_SYSTEM_NAME "Generic")

project (SimpleGit C)
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

IF (NOT LIBGIT2_BUILD_DIRECTORY_NAME)
	SET(LIBGIT2_BUILD_DIRECTORY_NAME build)
ENDIF()

set(TESTS_DIRECTORY ${ROOT_DIRECTORY}/tests)
set(LIBGIT2_DIRECTORY ${ROOT_DIRECTORY}/libgit2)
set(LIBGIT2_BUILD_DIRECTORY ${LIBGIT2_DIRECTORY}/${LIBGIT2_BUILD_DIRECTORY_NAME})
set(BINARIES_DIRECTORY ${ROOT_DIRECTORY}/bin)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${BINARIES_DIRECTORY})
set(EXECUTABLE_SOURCE_FILE ${SRC_DIRECTORY}/git.c)
set(UNIT_TESTS_DIRECTORY ${ROOT_DIRECTORY}/unittests)

file(GLOB_RECURSE C_SOURCE_FILES ${SRC_DIRECTORY}/*.c)
list(REMOVE_ITEM C_SOURCE_FILES ${EXECUTABLE_SOURCE_FILE})

#add_definitions(-DNO_PTHREADS)

SET(CMAKE_C_FLAGS "-Wno-missing-field-initializers -std=gnu99 -Wall -Wextra ${CMAKE_C_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "-g -O1")
set(CMAKE_C_FLAGS_RELEASE "-O2")

#include libssl
find_package(OpenSSL)
add_definitions(-DOPENSSL_SHA1 -DNO_OPENSSL)

set(CMAKE_BUILD_TYPE Debug)
#set(CMAKE_BUILD_TYPE Release)

include_directories(${INCLUDE_DIRECTORIES})
include_directories(${LIBGIT2_DIRECTORY}/include)
set(LIBS ${LIBS} git2.a pthread.a ${LIB_SHA1})
link_directories(${LIBGIT2_BUILD_DIRECTORY})

IF (AMIGA)
	include_directories(${ROOT_DIRECTORY}/libgit2/deps/regex)
	link_directories(${ROOT_DIRECTORY}/interim-openssl/openssl/repo/)
	set(LIBS ${LIBS} ssl.a crypto.a)
	set(LIBS ${LIBS} unix.a auto.a)
ELSE()
	set(LIBS ${LIBS} ssl crypto)
ENDIF()

# The extra git-test target involving the git test suite
add_custom_target(git-test-suite COMMAND $(MAKE) all WORKING_DIRECTORY ${TESTS_DIRECTORY} DEPENDS sgit-bin)
#add_custom_target(
#	build_libgit2
#	COMMAND
#		mkdir -p "${LIBGIT2_BUILD_DIRECTORY}" &&
#		cd "${LIBGIT2_BUILD_DIRECTORY}" &&
#		cmake -D BUILD_SHARED_LIBS:OPTION=OFF -D BUILD_TESTS:OPTION=ON -D CMAKE_BUILD_TYPE:STRING="${CMAKE_BUILD_TYPE}" .. &&
#		make
#)

add_library(sgit-lib STATIC ${C_SOURCE_FILES})
add_executable(sgit-bin ${EXECUTABLE_SOURCE_FILE})
set_target_properties(sgit-bin PROPERTIES OUTPUT_NAME sgit)
add_dependencies(sgit-bin build_libgit2)
target_link_libraries(sgit-bin sgit-lib ${LIBS})

IF (NOT AMIGA)
	enable_testing()

	add_executable(sgit-test ${UNIT_TESTS_DIRECTORY}/sgit-test.c)

	add_test(sgit-git ${EXECUTABLE_OUTPUT_PATH}/sgit-test)

	add_custom_target(update-readme
		COMMAND
			cd ${ROOT_DIRECTORY} &&
			devscripts/update-readme.py ${ROOT_DIRECTORY}/ReadMe.md >${ROOT_DIRECTORY}/ReadMe.md.temp &&
			mv ${ROOT_DIRECTORY}/ReadMe.md.temp ${ROOT_DIRECTORY}/ReadMe.md
		DEPENDS sgit-bin)
ENDIF()
