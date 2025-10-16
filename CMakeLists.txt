cmake_minimum_required(VERSION 3.21)

project(FirewoodBank VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Qt's automoc, autorcc, and autouic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS 
    Core
    Widgets 
    Sql
)

# Set output directories for better cross-platform support
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Platform-specific settings
if(WIN32)
    # Windows-specific settings
    set(CMAKE_WIN32_EXECUTABLE ON)
endif()

if(APPLE)
    # macOS-specific settings
    set(CMAKE_MACOSX_RPATH ON)
endif()

add_subdirectory(src)


