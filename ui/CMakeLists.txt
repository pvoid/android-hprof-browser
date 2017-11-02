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
project(hprof-browser C CXX)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0>=3.20 REQUIRED)

set(PROJECT_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/src/dispatcher.cxx
    ${PROJECT_SOURCE_DIR}/src/storage.cxx
    ${PROJECT_SOURCE_DIR}/src/hprof_storage.cxx
    ${PROJECT_SOURCE_DIR}/src/treeview_storage.cxx
    ${PROJECT_SOURCE_DIR}/src/hprof_browser_application.cxx
    ${PROJECT_SOURCE_DIR}/src/object_fields_columns.cxx
    ${PROJECT_SOURCE_DIR}/src/main_window.cxx
    ${PROJECT_SOURCE_DIR}/src/main.cxx
)

set(PROJECT_INCLUDE_DIRS
    ${hprof-library_INCLUDE_DIRS} 
    ${hprof-parser_INCLUDE_DIRS} 
    ${PROJECT_SOURCE_DIR}/includes
    ${GTKMM_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} hprof-library hprof-parser ${GTKMM_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

set(CMAKE_CXX_FLAGS -fexceptions)