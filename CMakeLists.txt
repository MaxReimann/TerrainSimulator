cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
project(terrainsimulator)

#################################################
# PROJECT DESCRIPTION
#################################################

set(META_PROJECT_NAME        "TerrainSimulator")
set(META_VERSION_MAJOR       "0")
set(META_VERSION_MINOR       "2")
set(META_VERSION_PATCH       "0")
set(META_VERSION             "${META_VERSION_MAJOR}.${META_VERSION_MINOR}.${META_VERSION_PATCH}")
set(META_AUTHOR_DOMAIN       "https://github.com/MaxReimann/TerrainSimulator")

#################################################
# OPTIONS
#################################################

option(OPTION_LIMIT_CONFIGS  "Generate limited configs (Release; Debug)" ON)
option(OPTION_LOCAL_INSTALL  "Install to a local directory instead of the system" OFF)
option(OPTION_ERRORS_AS_EXCEPTION "Throw exceptions" OFF)
option(OPTION_PORTABLE_INSTALL    "Install to a local directory instead of the system" ON)


#################################################
# CMAKE CONFIGURATION
#################################################

set(TERRAINSIM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Include cmake modules from ./cmake
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Find the WinSDK libs
if (WIN32)
	set(CMAKE_PREFIX_PATH "C:\\Program Files (x86)\\Windows Kits\\8.0\\Lib\\win8\\um\\x64" "C:\\Program Files (x86)\\Windows Kits\\8.1\\Lib\\winv6.3\\um\\x64")
	cmake_policy(SET CMP0020 NEW)
endif (WIN32)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Set configuration types
if(OPTION_LIMIT_CONFIGS)
	set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited Configs" FORCE)
endif()

# Project
project(${META_PROJECT_NAME} C CXX)

# Generate folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Include custom cmake functions
include(cmake/Custom.cmake)
include(cmake/GitRevision.cmake)

#################################################
# PLATFORM AND ARCHITECTURE
#################################################

# Architecture (32/64 bit)
set(X64 OFF)
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(X64 ON)
endif()

# Check for linux
if(UNIX AND NOT APPLE)
	set(LINUX 1)
endif()

# Setup platform specifics (compile flags, etc., ...)
if(MSVC)
	message(STATUS "Configuring for platform Windows/MSVC.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformWindowsMSVC.cmake)
elseif(WIN32 AND CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "Configuring for platform Windows/GCC.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformWindowsGCC.cmake)
elseif(LINUX AND CMAKE_COMPILER_IS_GNUCXX)
	message(STATUS "Configuring for platform Linux/GCC.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformLinuxGCC.cmake)
elseif(APPLE)
	message(STATUS "Configuring for platform MacOS.")
	include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/PlatformMacOS.cmake)
else()
	# Unsupported system/compiler
	message(WARNING "Unsupported platform/compiler combination")
endif()


#################################################
# DEPENDENCIES
#################################################

find_package(wxWidgets REQUIRED net gl core base)
include(${wxWidgets_USE_FILE})

# OpenGL
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(GLUT REQUIRED)


#################################################
# SOURCES
#################################################

set(sources

src/dcpanel.cpp
src/dcpanel.h
src/drawable.cpp
src/drawable.h
src/globals.h
src/image.cpp
src/image.h
src/stdafx.h
src/stdafx.cpp
src/loadobj.cpp
src/main.h
src/main.cpp
src/3dwindow.cpp
src/3dwindow.h
src/perlinnoise.cpp
src/perturbation.cpp
src/perturbation.h
src/primenumbers.h
src/resources.rc
src/simulation.cpp
src/simulation.h
src/toImage.cpp
src/voronoi.cpp
src/voronoi.h
src/watersimulation.cpp
src/watersimulation.h


src/util/tga.cpp
src/util/tga.h
src/util/mathlib.cpp
src/util/mathlib.h
src/util/random.h
src/util/random.cpp
)

set_source_files_properties(src/stdafx.cpp /Yc"stdafx.h")

source_group_by_path("${CMAKE_CURRENT_SOURCE_DIR}/source"
	"\\\\.h$|\\\\.hpp$|\\\\.cpp$|\\\\.c$|\\\\.ui$|\\\\.qrc$" "Source Files" ${sources})

#################################################
# TARGET
#################################################

# Set target name
set(target ${META_PROJECT_NAME})

add_executable(${target} ${sources})


if (MSVC)
   set_target_properties(${target} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(src/stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)

include_directories(
	${OPENGL_INCLUDE_DIR}
	${GLEW_INCLUDE_DIR}
	${GLUT_INCLUDE_DIR}
	${CMAKE_SOURCE_DIR}
	${wxWidgets_INCLUDE_DIRS}
	${PROJECT_BINARY_DIR} #for the generated files
)


message(STATUS ${GLEW_LIBRARY})

target_link_libraries( ${target}
    ${OPENGL_LIBRARIES}
    ${GLEW_LIBRARY}
    ${GLUT_LIBRARIES}
    D:\\Workspace\\libs\\freeglut-3.0.0\\lib\\lib\\freeglutd.lib

	${wxWidgets_LIBRARIES}
)
