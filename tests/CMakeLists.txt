add_executable(open_meteo_params_test
    params_test.cpp
)

target_link_libraries(open_meteo_params_test PRIVATE
    gtest_main
    open_meteo_models
)

target_compile_options(open_meteo_params_test PRIVATE -Wall -Wextra -Wpedantic)

add_test(NAME open_meteo_params_test COMMAND open_meteo_params_test)
