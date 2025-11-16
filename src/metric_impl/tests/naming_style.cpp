#include "metric_impl/naming_style.hpp"
#include "analyse.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {

class NamingStyleGroup : public testing::Test {
protected:
    void SetUp() override {
        testing::Test::SetUp();
        using namespace analyzer::metric::metric_impl;
        metricExtractor.RegisterMetric(std::make_unique<NamingStyleMetric>());
    }

    analyzer::metric::MetricExtractor metricExtractor{};
};

///
///
///

TEST_F(NamingStyleGroup, snakeStyle) {
    std::vector<std::string> files = {
        "../../../src/metric_impl/tests/files/many_parameters.py"
    };
    auto analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    auto values = analysis | std::views::values | std::views::join;

    auto isSnakeStyle = std::ranges::all_of(values, [](const metric::MetricResult &metric){
        return std::get<std::string>(metric.value) == "Snake Case";
    });
    EXPECT_TRUE(isSnakeStyle);
}

///
///
///

TEST_F(NamingStyleGroup, unknownCase) {
    std::vector<std::string> files = {
        "../../../src/metric_impl/tests/files/comments.py"
    };
    auto analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    auto values = analysis | std::views::values | std::views::join;

    auto isUnknown = std::ranges::all_of(values, [](const metric::MetricResult &metric){
        return std::get<std::string>(metric.value) == "Unknown";
    });
    EXPECT_TRUE(isUnknown);
}

///
///
///

TEST_F(NamingStyleGroup, camelStyle) {
    std::vector<std::string> files = {
        "../../../src/metric_impl/tests/files/if.py"
    };
    auto analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    auto values = analysis | std::views::values | std::views::join;

    auto isCamelStyle = std::ranges::all_of(values, [](const metric::MetricResult &metric){
        return std::get<std::string>(metric.value) == "Camel Case";
    });
    EXPECT_TRUE(isCamelStyle);
}

///
///
///

TEST_F(NamingStyleGroup, lowerStyle) {
    std::vector<std::string> files = {
        "../../../src/metric_impl/tests/files/many_lines.py"
    };
    auto analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    auto values = analysis | std::views::values | std::views::join;

    auto isCamelStyle = std::ranges::all_of(values, [](const metric::MetricResult &metric){
        return std::get<std::string>(metric.value) == "Lower Case";
    });
    EXPECT_TRUE(isCamelStyle);
}

///
///
///

TEST_F(NamingStyleGroup, pascalCase) {
    std::vector<std::string> files = {
        "../../../src/metric_impl/tests/files/nested_if.py"
    };
    auto analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    auto values = analysis | std::views::values | std::views::join;

    auto isPascalCase = std::ranges::all_of(values, [](const metric::MetricResult &metric){
        return std::get<std::string>(metric.value) == "Pascal Case";
    });
    EXPECT_TRUE(isPascalCase);
}

}  // namespace analyzer::metric::metric_impl
