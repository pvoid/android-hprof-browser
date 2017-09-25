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
project(hprof-parser C CXX)

find_package(BISON REQUIRED)

set(BISON_OUTPUT        ${PROJECT_SOURCE_DIR}/src/generated/language_parser.cxx)
set(BISON_OUTPUT_PARSER ${PROJECT_SOURCE_DIR}/includes/generated/language_parser.cxx)
set(BISON_OUTPUT_HEADER ${PROJECT_SOURCE_DIR}/includes/generated/language_parser.h)
add_custom_command(
    OUTPUT  ${BISON_OUTPUT_HEADER}
    COMMAND ${BISON_EXECUTABLE}
            --defines=${BISON_OUTPUT_HEADER}
            --output=${BISON_OUTPUT_PARSER}
            ${PROJECT_SOURCE_DIR}/src/language_parser.yy
    COMMENT "Generating language parser"
)

add_custom_command(
    OUTPUT ${BISON_OUTPUT}
    COMMAND cmake -E rename ${BISON_OUTPUT_PARSER} ${BISON_OUTPUT}
    DEPENDS ${BISON_OUTPUT_HEADER}
    COMMENT "Move language parser"
)

find_package(FLEX REQUIRED)
set(FLEX_OUTPUT ${PROJECT_SOURCE_DIR}/src/generated/language_scanner.cxx)
add_custom_command(
  OUTPUT ${FLEX_OUTPUT}
  COMMAND ${FLEX_EXECUTABLE}
          --outfile=${FLEX_OUTPUT}
          ${PROJECT_SOURCE_DIR}/src/language.lex
  COMMENT "Generating language scanner"
)

set(CMAKE_CXX_FLAGS -fexceptions)
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
"${PROJECT_SOURCE_DIR}/includes/generated/stack.hh;${PROJECT_SOURCE_DIR}/includes/generated/position.hh;${PROJECT_SOURCE_DIR}/includes/generated/location.hh;")

file(GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/src/*.cxx)

include_directories(${FLEX_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/includes/ ${PROJECT_SOURCE_DIR}/includes/generated/)

add_library(${PROJECT_NAME} ${SOURCE_FILES} ${FLEX_OUTPUT} ${BISON_OUTPUT})
target_link_libraries(${PROJECT_NAME} ${FLEX_LIBRARIES} ${BISON_LIBRARIES})

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)

set(${PROJECT_NAME}_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/includes/
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)
