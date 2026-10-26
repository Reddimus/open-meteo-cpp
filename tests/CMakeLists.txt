add_executable(open_meteo_params_test
    params_test.cpp
)

target_link_libraries(open_meteo_params_test PRIVATE
    gtest_main
    open_meteo_models
)

if(NOT MSVC)
    # GCC/Clang strict warnings; MSVC's analogous switches use
    # different syntax (/W4) and a different noise profile so we
    # leave the default there.
    target_compile_options(open_meteo_params_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME open_meteo_params_test COMMAND open_meteo_params_test)

# Glaze deserializer tests — verifies the migration's parse output matches
# the pre-migration shape on a representative payload.
add_executable(open_meteo_glaze_test
    glaze_test.cpp
)

target_link_libraries(open_meteo_glaze_test PRIVATE
    gtest_main
    open_meteo_models
)

if(NOT MSVC)
    target_compile_options(open_meteo_glaze_test PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_test(NAME open_meteo_glaze_test COMMAND open_meteo_glaze_test)

# Glaze parse-throughput regression guard. The bench parses a representative
# Open-Meteo response 1k times and asserts us/op stays under a cap. ctest
# --timeout = 30s; actual wall-clock is ~50ms on x86_64-v3, so the timeout
# fires only on catastrophic regression.
add_executable(open_meteo_parse_benchmark
    parse_benchmark.cpp
)
target_link_libraries(open_meteo_parse_benchmark PRIVATE
    open_meteo_models
)
if(NOT MSVC)
    target_compile_options(open_meteo_parse_benchmark PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(OPEN_METEO_ENABLE_SANITIZERS OR OPEN_METEO_ENABLE_COVERAGE)
    add_test(NAME open_meteo_parse_benchmark COMMAND open_meteo_parse_benchmark --no-timing)
else()
    add_test(NAME open_meteo_parse_benchmark COMMAND open_meteo_parse_benchmark $<$<CONFIG:Debug>:--no-timing>)
endif()
set_tests_properties(open_meteo_parse_benchmark PROPERTIES TIMEOUT 30)

add_executable(open_meteo_regression_test regression_test.cpp)
target_link_libraries(open_meteo_regression_test PRIVATE gtest_main open_meteo)
add_test(NAME open_meteo_regression_test COMMAND open_meteo_regression_test)

add_executable(open_meteo_http_test http_test.cpp)
target_link_libraries(open_meteo_http_test PRIVATE gtest_main open_meteo_http)
if(WIN32)
    target_link_libraries(open_meteo_http_test PRIVATE ws2_32)
    target_compile_definitions(open_meteo_http_test PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
endif()
add_test(NAME open_meteo_http_test COMMAND open_meteo_http_test)
