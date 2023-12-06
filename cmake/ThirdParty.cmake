include(ExternalProject)
include(ProcessorCount)

ProcessorCount(CPU_COUNT)

set(LIBPREFIX ${CMAKE_STATIC_LIBRARY_PREFIX})
set(LIBSUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})

# alsa
if(CMAKE_CROSSCOMPILING)
  ExternalProject_Add(alsa_lib
    URL "https://www.alsa-project.org/files/pub/lib/alsa-lib-1.0.29.tar.bz2"
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/alsa-src
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/alsa-build
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/alsa-prefix
    CONFIGURE_COMMAND
      ${CMAKE_COMMAND} -E env
        CC=${CMAKE_C_COMPILER}
        CCLD=${CMAKE_C_COMPILER}
        AR=${CMAKE_AR}
        RANLIB=${CMAKE_RANLIB}
        STRIP=${CMAKE_STRIP}
      ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/alsa-src/configure
        --host=${CMAKE_C_COMPILER_TARGET}
        --enable-shared
        --disable-static
        --disable-python
    BUILD_COMMAND
      make -j${CPU_COUNT}
    INSTALL_COMMAND
      make install DESTDIR=<INSTALL_DIR>
    LOG_DOWNLOAD YES
    LOG_CONFIGURE YES
    LOG_BUILD YES
    LOG_INSTALL YES
  )
  include_directories(SYSTEM
    ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/alsa-prefix/usr/include
  )
  link_libraries(
    ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/alsa-prefix/usr/lib/libasound.so
  )
else(CMAKE_CROSSCOMPILING)
  add_custom_target(alsa_lib)
  find_package(PkgConfig)
  pkg_check_modules(ALSA REQUIRED alsa)
  include_directories(SYSTEM
    ${ALSA_INCLUDE_DIRS}
  )
  link_libraries(
    ${ALSA_LIBRARIES}
  )
endif(CMAKE_CROSSCOMPILING)

# kissfft
ExternalProject_Add(kissfft_lib
  GIT_REPOSITORY "https://github.com/mborgerding/kissfft.git"
  GIT_TAG "131.1.0"
  GIT_SHALLOW ON
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/kissfft-src
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/kissfft-build
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/kissfft-prefix
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_C_COMPILER_TARGET=${CMAKE_C_COMPILER_TARGET}
    -DCMAKE_CXX_COMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_AR=${CMAKE_AR}
    -DCMAKE_RANLIB=${CMAKE_RANLIB}
    -DCMAKE_STRIP=${CMAKE_STRIP}
    -DCMAKE_POSITION_INDEPENDENT_CODE=ON
    -DKISSFFT_PKGCONFIG=OFF
    -DKISSFFT_STATIC=ON
    -DKISSFFT_TEST=OFF
    -DKISSFFT_TOOLS=OFF
    -DKISSFFT_USE_ALLOCA=OFF
  LOG_DOWNLOAD YES
  LOG_CONFIGURE YES
  LOG_BUILD YES
  LOG_INSTALL YES
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/kissfft-prefix/include/kissfft
)
link_libraries(
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/kissfft-prefix/lib/${LIBPREFIX}kissfft-float${LIBSUFFIX}
)

# concurrentqueue
ExternalProject_Add(concurrentqueue_lib
  GIT_REPOSITORY "https://github.com/cameron314/concurrentqueue.git"
  GIT_TAG "v1.0.3"
  GIT_SHALLOW ON
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/concurrentqueue-src
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD YES
  LOG_CONFIGURE YES
  LOG_BUILD YES
  LOG_INSTALL YES
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/concurrentqueue-src
)

# ispr
ExternalProject_Add(ispr_lib
  GIT_REPOSITORY "https://github.com/gershnik/intrusive_shared_ptr.git"
  GIT_TAG "v1.4"
  GIT_SHALLOW ON
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/ispr-src
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD YES
  LOG_CONFIGURE YES
  LOG_BUILD YES
  LOG_INSTALL YES
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/ispr-src/inc
)

# spdlog
ExternalProject_Add(spdlog_lib
  GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
  GIT_TAG "v1.12.0"
  GIT_SHALLOW ON
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog-src
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog-build
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog-prefix
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_C_COMPILER_TARGET=${CMAKE_C_COMPILER_TARGET}
    -DCMAKE_CXX_COMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_AR=${CMAKE_AR}
    -DCMAKE_RANLIB=${CMAKE_RANLIB}
    -DCMAKE_STRIP=${CMAKE_STRIP}
  LOG_DOWNLOAD YES
  LOG_CONFIGURE YES
  LOG_BUILD YES
  LOG_INSTALL YES
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog-prefix/include
)
link_libraries(
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/spdlog-prefix/lib/${LIBPREFIX}spdlog${LIBSUFFIX}
)

# cli11
ExternalProject_Add(cli11_lib
  GIT_REPOSITORY "https://github.com/CLIUtils/CLI11.git"
  GIT_TAG "v2.3.2"
  GIT_SHALLOW ON
  SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/cli11-src
  BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/cli11-build
  PREFIX ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/cli11-prefix
  CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
    -DCMAKE_C_COMPILER_TARGET=${CMAKE_C_COMPILER_TARGET}
    -DCMAKE_CXX_COMPILER_TARGET=${CMAKE_CXX_COMPILER_TARGET}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_AR=${CMAKE_AR}
    -DCMAKE_RANLIB=${CMAKE_RANLIB}
    -DCMAKE_STRIP=${CMAKE_STRIP}
    -DCLI11_BUILD_TESTS=OFF
    -DCLI11_BUILD_EXAMPLES=OFF
  LOG_DOWNLOAD YES
  LOG_CONFIGURE YES
  LOG_BUILD YES
  LOG_INSTALL YES
)
include_directories(SYSTEM
  ${CMAKE_CURRENT_BINARY_DIR}/3rdparty/cli11-prefix/include
)

# serialize dependencies
set(ALL_DEPENDENCIES
  alsa_lib
  kissfft_lib
  concurrentqueue_lib
  ispr_lib
  spdlog_lib
  cli11_lib
  )

list(REVERSE ALL_DEPENDENCIES)

set(OTHER_DEPENDENCIES ${ALL_DEPENDENCIES})
foreach(DEPENDENCY IN LISTS ALL_DEPENDENCIES)
  list(REMOVE_ITEM OTHER_DEPENDENCIES ${DEPENDENCY})
  if(OTHER_DEPENDENCIES)
    add_dependencies(${DEPENDENCY}
      ${OTHER_DEPENDENCIES}
      )
  endif()
endforeach()
