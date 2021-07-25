cmake_minimum_required(VERSION 3.0.0)

project(SignalEstimator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
     -Weverything \
     -Wno-system-headers \
     -Wno-old-style-cast \
     -Wno-switch-enum \
     -Wno-padded \
     -Wno-weak-vtables \
     -Wno-reserved-id-macro \
     -Wno-c++98-compat \
     -Wno-c++98-compat-pedantic \
     -Wno-c++98-c++11-c++14-compat")
endif()

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
     -Wall \
     -Wextra \
     -Wno-system-headers \
     -Wno-old-style-cast \
     -Wno-switch-enum")
endif()

include_directories(
  3rdparty/cxxopts/include
  src
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(signal-estimator
  src/cli/AlsaReader.cpp
  src/cli/AlsaUtils.cpp
  src/cli/AlsaWriter.cpp
  src/cli/ContinuousGenerator.cpp
  src/cli/FileDumper.cpp
  src/cli/Frame.cpp
  src/cli/GradientCounter.cpp
  src/cli/JSONFormatter.cpp
  src/cli/LatencyEstimator.cpp
  src/cli/LossEstimator.cpp
  src/cli/Main.cpp
  src/cli/RateLimiter.cpp
  src/cli/Realtime.cpp
  src/cli/RunMaxCounter.cpp
  src/cli/SchmittTrigger.cpp
  src/cli/SmaCounter.cpp
  src/cli/StrikeGenerator.cpp
  src/cli/TextFormatter.cpp
  src/cli/Time.cpp
)

find_package(Threads)

target_link_libraries(signal-estimator
  asound
  m
  ${CMAKE_THREAD_LIBS_INIT}
)

add_custom_command(TARGET signal-estimator POST_BUILD
  COMMAND "${CMAKE_COMMAND}" -E copy
     "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
     "${PROJECT_SOURCE_DIR}/compile_commands.json"
     COMMENT "Copying compile_commands.json to project root")
