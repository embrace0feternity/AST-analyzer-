#include "metric_impl/metrics.hpp"
#include "metric_accumulator_impl/sum_average_accumulator.hpp"
#include "analyse.hpp"
#include <gtest/gtest.h>

#include <cmath>

using namespace analyzer::metric::metric_impl;
namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

class SumAverageAccumulatorGroup : public testing::Test {
protected:
    void SetUp() override {
        testing::Test::SetUp();

        metricExtractor.RegisterMetric(std::make_unique<CountParametersMetric>());
        analysis = AnalyseFunctions(files, metricExtractor);
        accumulator.RegisterAccumulator(CountParametersMetric::kName, std::make_unique<SumAverageAccumulator>());
    }

    std::vector<std::string> files = { 
        "../../../src/metric_impl/tests/files/comments.py",
        "../../../src/metric_impl/tests/files/many_parameters.py",
        "../../../files/sample.py"
    };
    analyzer::metric::MetricExtractor metricExtractor{};

    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;

    MetricsAccumulator accumulator;
};

///
///
///

TEST_F(SumAverageAccumulatorGroup, parameters) 
{    
    analyzer::AccumulateFunctionAnalysis(analysis, accumulator);
    auto &accParameters =
            accumulator.template GetFinalizedAccumulator<SumAverageAccumulator>(CountParametersMetric::kName);
    
    /// No filter because there are only 1 type of metrics
    auto metrics = analysis | 
                std::views::values | 
                std::views::join;

    auto parameters = std::ranges::fold_left(metrics, 0, [](std::size_t init, const metric::MetricResult &metric){
        return init + std::get<int>(metric.value);;
    });
    auto sumAverage = accParameters.Get();
    EXPECT_DOUBLE_EQ(static_cast<double>(parameters) / analysis.size(), sumAverage.average);
    EXPECT_DOUBLE_EQ(parameters, sumAverage.sum);
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
