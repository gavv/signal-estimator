cmake_minimum_required(VERSION 3.5)

if(${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.24)
  cmake_policy(SET CMP0135 OLD)
endif()

project(SignalEstimator)

option(BUILD_GUI "build Qt GUI" ON)

option(ENABLE_SANITIZERS "enable sanitizers" OFF)
option(ENABLE_WERROR "enable -Werror" OFF)
option(BUILD_TESTING "Build tests" ON)

set(TOOLCHAIN_PREFIX "" CACHE STRING "target host triplet, e.g. aarch64-linux-gnu")
include("cmake/SetupToolchain.cmake")
include("cmake/ThirdParty.cmake")

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

list(APPEND COMPILER_FLAGS
  -pthread
  )
list(APPEND LINKER_FLAGS
  -pthread
  )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_compile_options(
    -fno-omit-frame-pointer
    -funwind-tables
    -ggdb
  )
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  list(APPEND COMPILER_FLAGS
    -Wall
    -Wextra
    # enable
    -Wcast-qual
    -Wctor-dtor-privacy
    -Wdouble-promotion
    -Wfloat-conversion
    -Wformat-security
    -Wformat=2
    -Wlogical-op
    -Wmissing-declarations
    -Woverlength-strings
    -Wpointer-arith
    -Wsign-conversion
    -Wstrict-null-sentinel
    -Wuninitialized
    # disable
    -Wno-old-style-cast
    -Wno-psabi
    -Wno-system-headers
    )
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND COMPILER_FLAGS
      -D_GLIBCXX_ASSERTIONS
      -D_GLIBCXX_DEBUG
      -D_GLIBCXX_SANITIZE_VECTOR
      )
  endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND COMPILER_FLAGS
    -Wall
    -Wextra
    # enable
    -Wcast-qual
    -Wdouble-promotion
    -Wfloat-conversion
    -Wformat-security
    -Wformat=2
    -Wnull-dereference
    -Woverlength-strings
    -Woverloaded-virtual
    -Wpointer-arith
    -Wsign-conversion
    -Wuninitialized
    -Wunused
    # disable
    -Wno-cast-align
    -Wno-system-headers
    -Wno-unused-private-field
    )
endif()

if(ENABLE_WERROR)
  list(APPEND COMPILER_FLAGS
    -Werror
    )
endif()

if(ENABLE_SANITIZERS)
  list(APPEND COMPILER_FLAGS
    -fsanitize=address
    -fsanitize=undefined
    )
  list(APPEND LINKER_FLAGS
    -fsanitize=address
    -fsanitize=undefined
    )
endif()

add_library(base_objects OBJECT
  src/base/core/Frame.cpp
  src/base/core/FramePool.cpp
  src/base/core/FrameQueue.cpp
  src/base/core/Log.cpp
  src/base/core/RateLimiter.cpp
  src/base/core/Realtime.cpp
  src/base/core/Sample.cpp
  src/base/core/Time.cpp
  src/base/dumps/AsyncDumper.cpp
  src/base/dumps/CsvDumper.cpp
  src/base/io/AlsaDeviceManager.cpp
  src/base/io/AlsaReader.cpp
  src/base/io/AlsaUtils.cpp
  src/base/io/AlsaWriter.cpp
  src/base/processing/ContinuousGenerator.cpp
  src/base/processing/CorrelationLatencyEstimator.cpp
  src/base/processing/IOJitterEstimator.cpp
  src/base/processing/Impulse.cpp
  src/base/processing/ImpulseGenerator.cpp
  src/base/processing/LossEstimator.cpp
  src/base/processing/StepsGenerator.cpp
  src/base/processing/StepsLatencyEstimator.cpp
  src/base/reports/JsonPrinter.cpp
  src/base/reports/JsonReporter.cpp
  src/base/reports/TextReporter.cpp
  src/base/run/Runner.cpp
)

add_dependencies(base_objects
  ${ALL_DEPENDENCIES}
  )

target_compile_options(base_objects
  PRIVATE ${COMPILER_FLAGS}
  )

target_link_options(base_objects
  PRIVATE ${LINKER_FLAGS}
  )

target_include_directories(base_objects
  PRIVATE src/base
  )

add_executable(signal-estimator
  src/cli/Formatter.cpp
  src/cli/Main.cpp
  )

add_dependencies(signal-estimator
  ${ALL_DEPENDENCIES}
  )

set(ALL_EXECUTABLES)
list(APPEND ALL_EXECUTABLES
  signal-estimator
  )

target_compile_options(signal-estimator
  PRIVATE ${COMPILER_FLAGS}
  )

target_link_options(signal-estimator
  PRIVATE ${LINKER_FLAGS}
  )

target_include_directories(signal-estimator
  PRIVATE src/base
  )

target_link_libraries(signal-estimator
  base_objects
  m
  )

install(
  TARGETS signal-estimator
  RUNTIME DESTINATION bin
  )

if(BUILD_GUI)
  find_package(Qt5Core REQUIRED)
  find_package(Qt5Widgets REQUIRED)
  find_package(Qwt REQUIRED)

  qt5_wrap_cpp(GENERATED_SOURCES
    src/gui/MainWindow.hpp
    src/gui/NotFoundDialog.hpp
    src/gui/SignalEstimator.hpp
    )

  set(UI_SOURCES
    src/gui/MainWindow.ui
    src/gui/NotFoundDialog.ui
    )

  foreach(UI_FILE IN LISTS UI_SOURCES)
    qt5_wrap_ui(GENERATED_SOURCES ${UI_FILE})
  endforeach()

  add_library(generated_objects OBJECT
    ${GENERATED_SOURCES}
    )

  add_dependencies(generated_objects
    ${ALL_DEPENDENCIES}
    )

  target_include_directories(generated_objects
    PRIVATE src/base
    )

  add_executable(signal-estimator-gui
    src/gui/Main.cpp
    src/gui/MainWindow.cpp
    src/gui/NotFoundDialog.cpp
    src/gui/PointsBuffer.cpp
    src/gui/RightClickPickerMachine.cpp
    src/gui/SignalEstimator.cpp
    )

  add_dependencies(signal-estimator-gui
    ${ALL_DEPENDENCIES}
    )

  list(APPEND ALL_EXECUTABLES
    signal-estimator-gui
    )

  target_compile_options(signal-estimator-gui
    PRIVATE ${COMPILER_FLAGS}
    )

  target_link_options(signal-estimator-gui
    PRIVATE ${LINKER_FLAGS}
    )

  target_include_directories(signal-estimator-gui
    PRIVATE src/base
    )

  foreach(TARGET IN ITEMS generated_objects signal-estimator-gui)
    target_include_directories(${TARGET} SYSTEM PRIVATE
      ${CMAKE_CURRENT_BINARY_DIR}
      ${QWT_INCLUDE_DIR}
      ${Qt5Core_INCLUDE_DIRS}
      ${Qt5Widgets_INCLUDE_DIRS}
      )
  endforeach()

  target_link_libraries(signal-estimator-gui
    generated_objects
    base_objects
    ${QWT_LIBRARY}
    Qt5::Core
    Qt5::Widgets
    )

  install(
    TARGETS signal-estimator-gui
    RUNTIME DESTINATION bin
    )
endif(BUILD_GUI)

set(BIN_DIR
  bin/${CMAKE_C_COMPILER_TARGET}
  )

set(INSTALLED_EXECUTABLES)

foreach(EXE_TARGET IN LISTS ALL_EXECUTABLES)
  set(EXE_PATH
    ${PROJECT_SOURCE_DIR}/${BIN_DIR}/${EXE_TARGET}${CMAKE_EXECUTABLE_SUFFIX}
  )
  add_custom_command(
    COMMENT "Copying ${EXE_TARGET} to ${BIN_DIR}"
    DEPENDS ${EXE_TARGET}
    OUTPUT ${EXE_PATH}
    COMMAND ${CMAKE_COMMAND} -E make_directory
      ${PROJECT_SOURCE_DIR}/${BIN_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy
      $<TARGET_FILE:${EXE_TARGET}>
      ${PROJECT_SOURCE_DIR}/${BIN_DIR}
    )
  list(APPEND INSTALLED_EXECUTABLES
    ${EXE_PATH}
    )
endforeach()

add_custom_target(install_executables ALL
  DEPENDS ${INSTALLED_EXECUTABLES}
)

if(NOT CMAKE_CROSSCOMPILING)
  add_custom_command(
    COMMENT "Copying compile_commands.json to project root"
    DEPENDS ${ALL_EXECUTABLES}
    OUTPUT ${PROJECT_SOURCE_DIR}/compile_commands.json
    COMMAND ${CMAKE_COMMAND} -E copy
      ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json
      ${PROJECT_SOURCE_DIR}/compile_commands.json
  )
  add_custom_target(compile_commands ALL
    DEPENDS ${PROJECT_SOURCE_DIR}/compile_commands.json
  )
endif()

if(BUILD_TESTING)

  enable_testing()
  include_directories("${CMAKE_BINARY_DIR}/googletest-src/googletest/include")
  link_directories("${CMAKE_BINARY_DIR}/googletest-build/lib")
  include("cmake/GTestConfig.cmake")
  
endif()