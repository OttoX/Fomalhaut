cmake_minimum_required(VERSION 3.5)

project(Fomalhaut LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

list(APPEND _sources 
	${CMAKE_CURRENT_LIST_DIR}/include/base_component.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/base_system.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/component_pool.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/entity_admin.cpp
	${CMAKE_CURRENT_LIST_DIR}/include/entity.cpp
	)

add_executable(example examples/example.cpp ${_sources})
add_executable(ecs_test test/ecs_test.cpp ${_sources})
enable_testing()
add_test(
  NAME catch_test
  COMMAND $<TARGET_FILE:ecs_test> --success
  )
