#include "metric_impl/parameters_count.hpp"
#include "analyse.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {


class ParametersCountGroup : public testing::Test {
protected:
    void SetUp() override {
        testing::Test::SetUp();
        using namespace analyzer::metric::metric_impl;
        metricExtractor.RegisterMetric(std::make_unique<CountParametersMetric>());
    }

    analyzer::metric::MetricExtractor metricExtractor{};
};

///
///
///

TEST_F(ParametersCountGroup, commentFunctionParameterCount) {
    /// Check only a file with comments
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/comments.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "Func_comments");
    EXPECT_EQ(result[0].metric_name, CountParametersMetric::kName);
    EXPECT_EQ(result[0].value, 3);
}

///
///
///

TEST_F(ParametersCountGroup, exceptionFunctionParameterCount) {
    /// Check only a file with comments
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/exceptions.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "Try_Exceptions");
    EXPECT_EQ(result[0].metric_name, CountParametersMetric::kName);
    EXPECT_EQ(result[0].value, 0);
}

///
///
///

TEST_F(ParametersCountGroup, manyParamFunctionParameterCount) {
    /// Check only a file with comments
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/many_parameters.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "__test_multiparameters__");
    EXPECT_EQ(result[0].metric_name, CountParametersMetric::kName);
    EXPECT_EQ(result[0].value, 5);
}

///
///
///

TEST_F(ParametersCountGroup, severalFilesParameterCount) {
    std::vector<std::string> files = { 
        "../../../src/metric_impl/tests/files/comments.py",
        "../../../src/metric_impl/tests/files/exceptions.py",
        "../../../src/metric_impl/tests/files/many_parameters.py"
    };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(3, analysis.size());

    auto values = analysis | std::views::values | std::views::join;

    auto complexity = std::ranges::fold_left(values, 0, [](std::size_t init, const metric::MetricResult &metric){
        return init + metric.value;
    });
    EXPECT_EQ(8, complexity);
}

}