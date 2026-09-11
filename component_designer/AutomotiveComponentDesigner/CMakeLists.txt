# *******************************************************************************
# Copyright (c) 2026 Contributors to the Eclipse Foundation
#
# See the NOTICE file(s) distributed with this work for additional
# information regarding copyright ownership.
#
# This program and the accompanying materials are made available under the
# terms of the Apache License Version 2.0 which is available at
# https://www.apache.org/licenses/LICENSE-2.0
#
# SPDX-License-Identifier: Apache-2.0
# *******************************************************************************

cmake_minimum_required(VERSION 3.16)

project(AutomotiveComponentDesigner
        VERSION 0.3.0
        DESCRIPTION "Editor for Eclipse autoapiframework function specifications"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

option(ACD_FETCH_WXWIDGETS "Download and build wxWidgets when no installation is found" ON)

# vcpkg and recent wxWidgets installations provide a package config file,
# system installations are found through CMake's FindwxWidgets module.
find_package(wxWidgets CONFIG QUIET COMPONENTS core base adv)
if(NOT wxWidgets_FOUND)
    find_package(wxWidgets QUIET COMPONENTS core base adv)
endif()

if(wxWidgets_FOUND)
    if(wxWidgets_USE_FILE)
        include(${wxWidgets_USE_FILE})
    endif()
    set(ACD_WX_LIBRARIES ${wxWidgets_LIBRARIES})
elseif(ACD_FETCH_WXWIDGETS)
    message(STATUS "wxWidgets not found - building it from source (this takes a while on the first run)")
    include(FetchContent)
    set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
    set(wxBUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(wxBUILD_SAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(wxWidgets
        GIT_REPOSITORY https://github.com/wxWidgets/wxWidgets.git
        GIT_TAG v3.2.6
        GIT_SHALLOW TRUE)
    FetchContent_MakeAvailable(wxWidgets)
    set(ACD_WX_LIBRARIES wx::core wx::base)
else()
    message(FATAL_ERROR
        "wxWidgets not found. Install it (vcpkg: 'vcpkg install wxwidgets', "
        "Linux: 'libwxgtk3.2-dev') or configure with -DACD_FETCH_WXWIDGETS=ON.")
endif()

add_executable(AutomotiveComponentDesigner WIN32
    src/main.cpp
    src/MainFrame.cpp
    src/YamlNode.cpp
    src/YamlParser.cpp
    src/YamlWriter.cpp
    src/MetaModel.cpp
    src/FunctionSpecification.cpp
)

target_include_directories(AutomotiveComponentDesigner PRIVATE src)
target_link_libraries(AutomotiveComponentDesigner PRIVATE ${ACD_WX_LIBRARIES})

if(MSVC)
    target_compile_options(AutomotiveComponentDesigner PRIVATE /W4 /utf-8)
    target_compile_definitions(AutomotiveComponentDesigner PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AutomotiveComponentDesigner)
else()
    target_compile_options(AutomotiveComponentDesigner PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(AutomotiveComponentDesigner PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:AutomotiveComponentDesigner>")

# The meta model and the sample specification are picked up next to the executable.
add_custom_command(TARGET AutomotiveComponentDesigner POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/../autoapiframework_meta_model.yaml"
            "$<TARGET_FILE_DIR:AutomotiveComponentDesigner>/autoapiframework_meta_model.yaml"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/../examples/speed_hazard_detection.acs"
            "$<TARGET_FILE_DIR:AutomotiveComponentDesigner>/speed_hazard_detection.acs"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/../examples/vehicle_speed_fusion_multirate.acs"
            "$<TARGET_FILE_DIR:AutomotiveComponentDesigner>/vehicle_speed_fusion_multirate.acs"            
    VERBATIM)

install(TARGETS AutomotiveComponentDesigner RUNTIME DESTINATION bin)
install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/../autoapiframework_meta_model.yaml" DESTINATION bin)
