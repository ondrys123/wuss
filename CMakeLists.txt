cmake_minimum_required(VERSION 3.16.3 FATAL_ERROR)

# Project settings
project(wuss 
    VERSION 0.0.1 
    DESCRIPTION  "PV204 Semestral project 2021"
    HOMEPAGE_URL "https://github.com/ondrys123/wuss"
    LANGUAGES C CXX
)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add FindSGX.cmake
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(Dependencies)

# Set Simulation and Debug mode
set(SGX_HW OFF CACHE BOOL "Enable HW mode" FORCE)
set(SGX_MODE "Debug" CACHE STRING "SGX mode" FORCE)

# Check if path to SGX SDK is defined, add SGX package
find_package(SGX REQUIRED)

# Add project subdirectories
add_subdirectory(misc)
add_subdirectory(trusted)
add_subdirectory(untrusted)
add_subdirectory(tests)

# Source files for wuss executable
set(main_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${untrusted_files}
)

# Create executable
add_untrusted_executable(wuss
    SRCS ${main_sources}
    EDL ${enclave_edl_file}
    EDL_SEARCH_PATHS ${enclave_edl_dir}
)

# Add signed lib as dependency
add_dependencies(wuss ${signed_target})
target_link_libraries(wuss PUBLIC Boost::program_options)

# Define "trusted/trusted.signed.so"
target_compile_definitions(wuss PUBLIC SIGNED_LIB="trusted/${signed_lib}")


# Unused variables
if(QT_QMAKE_EXECUTABLE)
endif()
