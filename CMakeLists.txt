cmake_minimum_required(VERSION 3.20)
project(open-meteo-cpp VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Options
option(OPEN_METEO_BUILD_TESTS "Build tests" ON)
option(OPEN_METEO_BUILD_EXAMPLES "Build examples" ON)
option(OPEN_METEO_ENABLE_LTO "Enable Link Time Optimization" ON)
option(OPEN_METEO_NATIVE_ARCH "Use -march=native for CPU-specific tuning" OFF)

# Compiler warnings
add_compile_options(-Wall -Wextra -Wpedantic)

# Aggressive optimization flags for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Enable LTO if requested and supported
if(OPEN_METEO_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LTO_SUPPORTED OUTPUT LTO_ERROR)
    if(LTO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
        message(STATUS "LTO enabled for Release builds")
    else()
        message(WARNING "LTO not supported: ${LTO_ERROR}")
    endif()
endif()

# CPU-specific tuning (optional, not portable)
if(OPEN_METEO_NATIVE_ARCH)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")
    message(STATUS "Using -march=native for CPU-specific optimizations")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mtune=generic")
endif()

# Sanitizer support
option(OPEN_METEO_ENABLE_SANITIZERS "Enable AddressSanitizer and UBSanitizer" OFF)
if(OPEN_METEO_ENABLE_SANITIZERS)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
    message(STATUS "Sanitizers enabled (ASan + UBSan)")
endif()

# Code coverage support
option(OPEN_METEO_ENABLE_COVERAGE "Enable code coverage instrumentation (gcov)" OFF)
if(OPEN_METEO_ENABLE_COVERAGE)
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
    message(STATUS "Code coverage enabled")
endif()

# Find dependencies
find_package(CURL REQUIRED)

include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)
set(JSON_BuildTests OFF CACHE INTERNAL "")
FetchContent_MakeAvailable(json)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add subdirectories
add_subdirectory(src)

if(OPEN_METEO_BUILD_TESTS)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.15.2
        GIT_SHALLOW TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    add_subdirectory(tests)
endif()

if(OPEN_METEO_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Install targets
set(OPEN_METEO_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/open_meteo")

install(TARGETS open_meteo_core open_meteo_http open_meteo_models open_meteo_api open_meteo
    EXPORT open_meteoTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(DIRECTORY include/open_meteo DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/open_meteoConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/open_meteoConfig.cmake
    INSTALL_DESTINATION ${OPEN_METEO_INSTALL_CMAKEDIR}
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/open_meteoConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
export(EXPORT open_meteoTargets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/open_meteoTargets.cmake
    NAMESPACE open_meteo::
)
install(EXPORT open_meteoTargets
    FILE open_meteoTargets.cmake
    NAMESPACE open_meteo::
    DESTINATION ${OPEN_METEO_INSTALL_CMAKEDIR}
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/open_meteoConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/open_meteoConfigVersion.cmake
    DESTINATION ${OPEN_METEO_INSTALL_CMAKEDIR}
)
