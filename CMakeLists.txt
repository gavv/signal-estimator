cmake_minimum_required(VERSION 3.0.0)

project(SignalEstimator)

option(BUILD_GUI "Build Qt GUI" ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  list(APPEND COMPILER_FLAGS
    "-Wall"
    "-Wextra"
    "-Wno-cast-align"
    )
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  list(APPEND COMPILER_FLAGS
    "-Wall"
    "-Wextra"
    "-Wno-old-style-cast"
    "-Wno-switch-enum"
    "-Wno-system-headers"
    )
endif()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(signal-estimator
  src/cli/Main.cpp
  src/cli/core/Frame.cpp
  src/cli/core/FramePool.cpp
  src/cli/core/Realtime.cpp
  src/cli/core/Time.cpp
  src/cli/fmt/AsyncDumper.cpp
  src/cli/fmt/CsvDumper.cpp
  src/cli/fmt/JsonFormatter.cpp
  src/cli/fmt/TextFormatter.cpp
  src/cli/processing/ContinuousGenerator.cpp
  src/cli/processing/CorrelationLatencyEstimator.cpp
  src/cli/processing/FFTOps.cpp
  src/cli/processing/Impulse.cpp
  src/cli/processing/ImpulseGenerator.cpp
  src/cli/processing/LossEstimator.cpp
  src/cli/processing/RateLimiter.cpp
  src/cli/processing/StepsGenerator.cpp
  src/cli/processing/StepsLatencyEstimator.cpp
  src/cli/sndio/AlsaReader.cpp
  src/cli/sndio/AlsaUtils.cpp
  src/cli/sndio/AlsaWriter.cpp
  )

target_compile_options(signal-estimator
  PRIVATE ${COMPILER_FLAGS}
  )

function(checkout_submodule DIRECTORY)
  set(ABS_DIRECTORY "${PROJECT_SOURCE_DIR}/${DIRECTORY}")
  if(NOT EXISTS ${ABS_DIRECTORY}/.git)
    if(GIT_EXECUTABLE STREQUAL "")
      find_package(Git REQUIRED)
    endif()
    message(STATUS "Checking out ${DIRECTORY}...")
    execute_process(
      COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive .
      WORKING_DIRECTORY ${ABS_DIRECTORY}
      RESULT_VARIABLE GIT_EXIT_CODE
      )
    if(NOT GIT_EXIT_CODE EQUAL "0")
      message(FATAL_ERROR "Failed to check out ${DIRECTORY}")
    endif()
  endif()
endfunction(checkout_submodule)

checkout_submodule(3rdparty/concurrentqueue)
checkout_submodule(3rdparty/cxxopts)
checkout_submodule(3rdparty/kissfft)

option(KISSFFT_PKGCONFIG "" OFF)
option(KISSFFT_STATIC "" ON)
option(KISSFFT_TEST "" OFF)
option(KISSFFT_TOOLS "" OFF)
option(KISSFFT_USE_ALLOCA "" OFF)
add_subdirectory(3rdparty/kissfft)
target_compile_options(kissfft
  PRIVATE -w
  )

target_include_directories(signal-estimator SYSTEM
  PRIVATE 3rdparty/concurrentqueue
  PRIVATE 3rdparty/cxxopts/include
  PRIVATE 3rdparty/kissfft
  PRIVATE src/cli
  )

find_package(Threads)
target_link_libraries(signal-estimator
  asound
  m
  kissfft
  ${CMAKE_THREAD_LIBS_INIT}
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

  add_executable(signal-estimator-gui
    src/gui/DeviceManager.cpp
    src/gui/Main.cpp
    src/gui/MainWindow.cpp
    src/gui/NotFoundDialog.cpp
    src/gui/PointsBuffer.cpp
    src/gui/RightClickPickerMachine.cpp
    src/gui/SignalEstimator.cpp
    )

  target_compile_options(signal-estimator-gui
    PRIVATE ${COMPILER_FLAGS}
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
    ${QWT_LIBRARY}
    Qt5::Core
    Qt5::Widgets
    kissfft
    )

  install(
    TARGETS signal-estimator-gui
    RUNTIME DESTINATION bin
    )
endif(BUILD_GUI)

add_custom_command(TARGET signal-estimator POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
     "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
     "${PROJECT_SOURCE_DIR}/compile_commands.json"
     COMMENT "Copying compile_commands.json to project root"
     )
