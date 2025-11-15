#include "metric_impl/cyclomatic_complexity.hpp"
#include "analyse.hpp"
#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {


class CyclomaticComplexityGroup : public testing::Test {
protected:
    void SetUp() override {
        testing::Test::SetUp();
        using namespace analyzer::metric::metric_impl;
        metricExtractor.RegisterMetric(std::make_unique<CyclomaticComplexityMetric>());
    }

    analyzer::metric::MetricExtractor metricExtractor{};
};

///
///
///

TEST_F(CyclomaticComplexityGroup, commentFunctionComplexity) {
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/comments.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "Func_comments");
    EXPECT_EQ(result[0].metric_name, CyclomaticComplexityMetric::kName);
    EXPECT_EQ(result[0].value, 1);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, exceptionFunctionComplexity) {
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/exceptions.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "Try_Exceptions");
    EXPECT_EQ(result[0].metric_name, CyclomaticComplexityMetric::kName);
    /// Try, finally
    EXPECT_EQ(result[0].value, 3);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, ifFunctionComplexity) {
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/if.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "testIf");
    EXPECT_EQ(result[0].metric_name, CyclomaticComplexityMetric::kName);
    /// Only 1 if
    EXPECT_EQ(result[0].value, 2);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, loopsFunctionComplexity) {
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/loops.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "TestLoops");
    EXPECT_EQ(result[0].metric_name, CyclomaticComplexityMetric::kName);
    /// For, while, if
    EXPECT_EQ(result[0].value, 4);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, switchFunctionComplexity) {
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/match_case.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "test_Match_case");
    EXPECT_EQ(result[0].metric_name, CyclomaticComplexityMetric::kName);
    /// 3 cases
    EXPECT_EQ(result[0].value, 4);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, nestedIfFunctionComplexity) {
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/nested_if.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "Testnestedif");
    EXPECT_EQ(result[0].metric_name, CyclomaticComplexityMetric::kName);
    /// if, nested if, elseif
    EXPECT_EQ(result[0].value, 4);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, ternaryFunctionComplexity) {
    std::vector<std::string> files{ "../../../src/metric_impl/tests/files/ternary.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "teSt_ternary");
    EXPECT_EQ(result[0].metric_name, CyclomaticComplexityMetric::kName);
    /// ternary operator, nested ternary operator
    EXPECT_EQ(result[0].value, 3);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, functionName) {
    /// When a function name is similar to tree-sitter's node type names
    std::vector<std::string> files{ "../../../files/math.py" };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(4, analysis.size());

    /// No filter because there are only 1 type of metrics
    auto metrics = analysis | 
                std::views::values | 
                std::views::join;

    auto complexity = std::ranges::fold_left(metrics, 0, [](std::size_t init, const metric::MetricResult &metric){
        return init + metric.value;
    });

    EXPECT_EQ(complexity, 5);
}

///
///
///

TEST_F(CyclomaticComplexityGroup, allFunctionsComplexity) {
    std::vector<std::string> files = { 
        "../../../src/metric_impl/tests/files/comments.py",
        "../../../src/metric_impl/tests/files/exceptions.py",
        "../../../src/metric_impl/tests/files/if.py",
        "../../../src/metric_impl/tests/files/loops.py",
        "../../../src/metric_impl/tests/files/match_case.py",
        "../../../src/metric_impl/tests/files/nested_if.py",
        "../../../src/metric_impl/tests/files/ternary.py"
    };

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(7, analysis.size());

    auto values = analysis | std::views::values | std::views::join;

    auto complexity = std::ranges::fold_left(values, 0, [](std::size_t init, const metric::MetricResult &metric){
        return init + metric.value;
    });
    EXPECT_EQ(21, complexity);
}

}  // namespace analyzer::metric::metric_impl
