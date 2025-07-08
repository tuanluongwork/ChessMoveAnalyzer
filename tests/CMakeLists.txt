# Find Google Test
find_package(GTest REQUIRED)

# Test sources
set(TEST_SOURCES
    test_main.cpp
    test_position.cpp
    test_move_generation.cpp
    test_move_explainer.cpp
)

# Create test executable
add_executable(run_tests ${TEST_SOURCES})

# Link with chess analyzer library and Google Test
target_link_libraries(run_tests 
    chess_analyzer
    GTest::GTest
    GTest::Main
)

# Add test
add_test(NAME ChessAnalyzerTests COMMAND run_tests)

# Copy test data if needed
file(COPY test_data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 