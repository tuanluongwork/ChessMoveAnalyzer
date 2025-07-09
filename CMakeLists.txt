cmake_minimum_required(VERSION 3.14)
project(ChessMoveAnalyzer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -O3 -march=native)
elseif(MSVC)
    add_compile_options(/W4 /O2)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

# Create library
add_library(chess_analyzer STATIC ${SOURCES})

# Create executable for CLI tool
add_executable(chess-analyzer-cli 
    ${CMAKE_CURRENT_SOURCE_DIR}/tools/cli/main.cpp
)
target_link_libraries(chess-analyzer-cli chess_analyzer)
set_target_properties(chess-analyzer-cli PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    OUTPUT_NAME chess-analyzer
)

# Testing (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest)
    if(GTest_FOUND)
        add_subdirectory(tests)
    else()
        message(WARNING "Google Test not found, tests will not be built")
    endif()
endif()

# Examples (optional)
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples/analysis_demo.cpp)
    add_executable(analysis_demo ${CMAKE_CURRENT_SOURCE_DIR}/examples/analysis_demo.cpp)
    target_link_libraries(analysis_demo chess_analyzer)
    set_target_properties(analysis_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Benchmarks (optional)
option(BUILD_BENCHMARKS "Build benchmarks" ON)
if(BUILD_BENCHMARKS AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/perft.cpp)
    add_executable(perft ${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/perft.cpp)
    target_link_libraries(perft chess_analyzer)
    set_target_properties(perft PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Documentation (optional)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    
    if(EXISTS ${DOXYGEN_IN})
        configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
        
        add_custom_target(docs
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
    endif()
endif()

# Installation
install(TARGETS chess_analyzer
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/ DESTINATION include)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/ChessMoveAnalyzerConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
) 