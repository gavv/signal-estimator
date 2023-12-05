
add_executable(SignalEstimatorTests
    TextReporterTests.cpp
)

add_dependencies(SignalEstimatorTests googletest)

target_link_libraries(SignalEstimatorTests
  PRIVATE
    base_objects
    ${CMAKE_THREAD_LIBS_INIT}
    gtest
    gtest_main
)

target_include_directories(SignalEstimatorTests
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../base
)

gtest_discover_tests(SignalEstimatorTests)
