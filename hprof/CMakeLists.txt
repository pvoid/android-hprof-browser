#
#  Copyright 2017 Dmitry "PVOID" Petukhov
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
cmake_minimum_required(VERSION 3.5)
project(hprof-library C CXX)

set(PROJECT_SOURCE_FILES 
    ${PROJECT_SOURCE_DIR}/src/types.cxx
    ${PROJECT_SOURCE_DIR}/src/types/class.cxx
    ${PROJECT_SOURCE_DIR}/src/types/gc_root.cxx
    ${PROJECT_SOURCE_DIR}/src/types/instance.cxx
    ${PROJECT_SOURCE_DIR}/src/types/string_instance.cxx
    ${PROJECT_SOURCE_DIR}/src/types/objects_array.cxx
    ${PROJECT_SOURCE_DIR}/src/types/primitives_array.cxx
    ${PROJECT_SOURCE_DIR}/src/reader/data_reader_v103.cxx
    ${PROJECT_SOURCE_DIR}/src/hprof_file.cxx
    ${PROJECT_SOURCE_DIR}/src/data_reader_factory.cxx
    ${PROJECT_SOURCE_DIR}/src/heap_profile.cxx
)
set(PROJECT_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/includes/)

add_library(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE_DIRS})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/includes/
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(PROJECT_TEST_SOURCE_FILES 
    ${PROJECT_SOURCE_DIR}/test/main.cxx)

set(PROJECT_TEST_INCLUDE_DIRS $
    ${PROJECT_INCLUDE_DIRS} 
    ${GTEST_INCLUDE_DIRS} 
    ${PROJECT_SOURCE_DIR}/test/)

add_executable(${PROJECT_NAME}-test ${PROJECT_SOURCE_FILES} ${PROJECT_TEST_SOURCE_FILES})
add_definitions(-DTEST_DATA_DIR="${PROJECT_SOURCE_DIR}/test-data")
target_include_directories(${PROJECT_NAME}-test PRIVATE ${PROJECT_TEST_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}-test ${CMAKE_THREAD_LIBS_INIT} ${GTEST_LIBRARIES} ${GMOCK_LIBRARIES})

set_property(TARGET ${PROJECT_NAME}-test PROPERTY CXX_STANDARD 14)
set_target_properties(${PROJECT_NAME}-test PROPERTIES COMPILE_FLAGS "-fprofile-arcs -ftest-coverage")
set_target_properties(${PROJECT_NAME}-test PROPERTIES LINK_FLAGS "--coverage")

add_test(${PROJECT_NAME} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}-test)

add_custom_target(${PROJECT_NAME}-coverage
   COMMAND ${LCOV} --directory . --zerocounters
   COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}-test
   COMMAND ${LCOV} --no-external --directory . --base-directory ${PROJECT_SOURCE_DIR} --gcov-tool ${CMAKE_SOURCE_DIR}/tools/llvm-gcov --capture -o ./${PROJECT_NAME}-coverage-report.info
   COMMAND ${LCOV} --remove ./${PROJECT_NAME}-coverage-report.info '*/test/*' -o ./${PROJECT_NAME}-coverage-report.info
   COMMAND ${GENHTML} ./${PROJECT_NAME}-coverage-report.info -o ${CMAKE_BINARY_DIR}/reports/${PROJECT_NAME}/

   DEPENDS ${PROJECT_NAME}-test
   COMMENT "Collecting code coverage data"
)
