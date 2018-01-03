cmake_minimum_required(VERSION 2.8)

set( PROJ_NAME      "Fomalhaut" )
set( PROJ_PATH      ${CMAKE_SOURCE_DIR} )
set( PROJ_OUT_PATH  ${CMAKE_BINARY_DIR} )
set( PROJ_SOURCES   "")
set( PROJ_HEADERS   "")
set( PROJ_TEST_SOURCES "")
set( PROJ_TEST_HEADERS "")

set( CMAKE_MODULE_PATH "${PROJ_PATH}/cmake")
set( EXECUTABLE_OUTPUT_PATH ${PROJ_OUT_PATH})

set( PROJ_LIB_PATH  "${PROJ_PATH}/lib" )
set( PROJ_INCLUDES  "${PROJ_PATH}/include")
set( PROJ_TEST_INCLUDES "${PROJ_PATH}/test")
set( LIB_INCLUDES "${PROJ_LIB_PATH}/include")

project(${PROJ_NAME})
include(ExternalProject)

# Default compiler args
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "(GNU|.*Clang)")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Werror -Wall -Wextra -Wno-unused-parameter -Wno-error=unused-variable -Wno-error=sign-compare -std=c++11")
	set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
	set(CMAKE_CXX_FLAGS_MINSIZEREL "-g -Os -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELEASE "-g -O2 -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	# /Zi - Produces a program database (PDB) that contains type information and symbolic debugging information for use with the debugger.
	# /FS - Allows multiple cl.exe processes to write to the same .pdb file
	# /DEBUG - Enable debug during linking
	# /Od - Disables optimization
	set(CMAKE_CXX_FLAGS_DEBUG "/Zi /FS /DEBUG /Od /MDd")
	# /Ox - Full optimization
	set(CMAKE_CXX_FLAGS_RELEASE "/Ox -DNDEBUG")
	set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/Ox /Zi /FS /DEBUG")
endif()

if (NOT CMAKE_BUILD_TYPE)
    message("-- Defaulting to release build (use -DCMAKE_BUILD_TYPE:STRING=Debug for debug build)")
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Adds sources and headers to project
file( GLOB_RECURSE PROJ_SOURCES include/*.cpp)
file( GLOB_RECURSE PROJ_HEADERS include/*.h include/*.inl)
file( GLOB_RECURSE PROJ_TEST_SOURCES test/common/*.cpp )
file( GLOB_RECURSE PROJ_TEST_HEADERS test/common/*.hpp )
include_directories( ${PROJ_INCLUDES} ${LIB_INCLUDES} ${PROJ_TEST_INCLUDES})

# Tests
enable_testing()
add_executable( UnitTests ${PROJ_TEST_SOURCES} test/ecs.cpp ${PROJ_TEST_HEADERS} ${PROJ_HEADERS})
add_executable( Example examples/example.cpp ${PROJ_SOURCES} ${PROJ_HEADERS})

add_test( UnitTests UnitTests)

install(TARGETS UnitTests DESTINATION ${PROJ_OUT_PATH})
install(TARGETS Example DESTINATION ${PROJ_OUT_PATH})

