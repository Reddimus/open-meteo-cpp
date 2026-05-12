# Core library (error, rate limiting, retry)
#
# Glaze is header-only and pulled in via include-path only (matching how
# nlohmann was integrated pre-migration). This sidesteps CMake's "INTERFACE
# target not in export set" error that fires if we link glaze::glaze
# directly onto these installable static libs.
add_library(open_meteo_core STATIC
    core/error.cpp
    core/rate_limit.cpp
    core/retry.cpp
)
target_compile_features(open_meteo_core PUBLIC cxx_std_23)
target_include_directories(open_meteo_core PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${glaze_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# HTTP client library
add_library(open_meteo_http STATIC
    http/client.cpp
)
target_link_libraries(open_meteo_http PUBLIC open_meteo_core CURL::libcurl)
target_include_directories(open_meteo_http PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Models library (common types, params, geocoding, elevation, seasonal)
add_library(open_meteo_models STATIC
    models/common.cpp
    models/elevation.cpp
    models/geocoding.cpp
    models/params.cpp
    models/seasonal.cpp
)
target_link_libraries(open_meteo_models PUBLIC open_meteo_core)
target_include_directories(open_meteo_models PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${glaze_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# API client library
add_library(open_meteo_api STATIC
    api/client.cpp
)
target_link_libraries(open_meteo_api PUBLIC open_meteo_core open_meteo_http open_meteo_models)
target_include_directories(open_meteo_api PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# Main SDK library (combines all)
add_library(open_meteo INTERFACE)
target_link_libraries(open_meteo INTERFACE open_meteo_core open_meteo_http open_meteo_models open_meteo_api)
