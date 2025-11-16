#include "metric_impl/metrics.hpp"
#include "metric_accumulator_impl/average_accumulator.hpp"
#include "analyse.hpp"
#include <gtest/gtest.h>

#include <cmath>

using namespace analyzer::metric::metric_impl;
namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

class AverageAccumulatorGroup : public testing::Test {
protected:
    void SetUp() override {
        testing::Test::SetUp();

        metricExtractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());
        metricExtractor.RegisterMetric(std::make_unique<CyclomaticComplexityMetric>());

        analysis = AnalyseFunctions(files, metricExtractor);

        accumulator.RegisterAccumulator(CyclomaticComplexityMetric::kName, std::make_unique<AverageAccumulator>());
        accumulator.RegisterAccumulator(CodeLinesCountMetric::kName, std::make_unique<AverageAccumulator>());

        analyzer::AccumulateFunctionAnalysis(analysis, accumulator);
    }

    std::vector<std::string> files = { 
        /// Code lines - 3
        /// Complexity - 1
        "../../../src/metric_impl/tests/files/comments.py",
        /// Code lines - 1
        /// Complexity - 1
        "../../../src/metric_impl/tests/files/many_parameters.py",
        /// Code lines - 13
        /// Complexity - 4
        "../../../files/sample.py"
    };
    analyzer::metric::MetricExtractor metricExtractor{};

    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;

    MetricsAccumulator accumulator;
};

///
///
///

TEST_F(AverageAccumulatorGroup, codeLines) 
{    
    auto &accCodeCount =
            accumulator.template GetFinalizedAccumulator<AverageAccumulator>(CodeLinesCountMetric::kName);
    
    auto metrics = analysis | 
                std::views::values | 
                std::views::join |
                std::views::filter([](metric::MetricResult &metric){
                    return metric.metric_name == CodeLinesCountMetric::kName;
                });

    auto codeLines = std::ranges::fold_left(metrics, 0, [](std::size_t init, const metric::MetricResult &metric){
        return init + std::get<int>(metric.value);
    });
    EXPECT_DOUBLE_EQ(static_cast<double>(codeLines) / analysis.size(), accCodeCount.Get());

    /// Change all the metrics manually to see how IAccumulator's reset() 
    ///  method works
    /// Make each i-th value of the codeLines equal to analysis.size()
    accumulator.ResetAccumulators();

    std::ranges::for_each(
        metrics, 
        [newValue = analysis.size()](auto &v){ std::get<int>(v) = newValue; }, 
        &metric::MetricResult::value
    );

    analyzer::AccumulateFunctionAnalysis(analysis, accumulator);
    accumulator.template GetFinalizedAccumulator<AverageAccumulator>(CodeLinesCountMetric::kName);
    EXPECT_DOUBLE_EQ(6., accCodeCount.Get());
}

///
///
///

TEST_F(AverageAccumulatorGroup, complexity) 
{    
    auto &accComplexity =
            accumulator.template GetFinalizedAccumulator<AverageAccumulator>(CyclomaticComplexityMetric::kName);
    
    auto metrics = analysis | 
                std::views::values | 
                std::views::join |
                std::views::filter([](metric::MetricResult &metric){
                    return metric.metric_name == CyclomaticComplexityMetric::kName;
                });

    auto complexity = std::ranges::fold_left(metrics, 0, [](std::size_t init, const metric::MetricResult &metric){
        return init + std::get<int>(metric.value);;
    });
    EXPECT_DOUBLE_EQ(static_cast<double>(complexity) / analysis.size(), accComplexity.Get());
    EXPECT_EQ(6, complexity);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
