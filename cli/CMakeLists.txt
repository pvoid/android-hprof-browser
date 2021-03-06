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
project(hprof-cli C CXX)

set(PROJECT_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/main.cxx
    ${PROJECT_SOURCE_DIR}/src/tools.cxx
)

set(PROJECT_INCLUDE_DIRS
    ${hprof-library_INCLUDE_DIRS}
    ${hprof-parser_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/includes
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} hprof-library hprof-parser)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
