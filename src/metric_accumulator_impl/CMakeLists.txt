add_executable(metric_accumulator_test
    tests/average_accumulator.cpp
    tests/categorical_accumulator.cpp
    tests/sum_average_accumulator.cpp
)

target_link_libraries(metric_accumulator_test
    PRIVATE
        GTest::GTest
        GTest::Main
        metric_accumulator
        metric
)

add_test(NAME metric_accumulator_test COMMAND metric_accumulator_test)