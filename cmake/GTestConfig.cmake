  find_package(GTest REQUIRED)
  include_directories(${GTEST_INCLUDE_DIRS})
  include_directories(src/base/processing)
  include_directories(googletest/src)

  add_executable(gtests googletest/gtest_main.cpp)

  target_link_libraries(signal-estimator ${GTEST_LIBRARIES})
  target_link_libraries(gtests ${GTEST_LIBRARIES})

  add_test(NAME ProcessingTests COMMAND gtests)