#include "metric_accumulator_impl/categorical_accumulator.hpp"
#include "metric_impl/metrics.hpp"
#include "analyse.hpp"
#include <gtest/gtest.h>

#include <stdexcept>

using namespace analyzer::metric::metric_impl;
namespace analyzer::metric_accumulator::metric_accumulator_impl::test {

class CategorialAccumulatorGroup : public testing::Test {
protected:
    void SetUp() override {
        testing::Test::SetUp();

        metricExtractor.RegisterMetric(std::make_unique<NamingStyleMetric>());

        analysis = AnalyseFunctions(files, metricExtractor);
        accumulator.RegisterAccumulator(NamingStyleMetric::kName, std::make_unique<CategoricalAccumulator>());
    }

    std::vector<std::string> files = {
        /// snake  - 3
        /// pascal - 0
        /// lower  - 1
        /// camel  - 0
        "../../../files/sample.py",
        /// snake  - 1
        /// pascal - 1
        /// lower  - 1
        /// camel  - 1
        "../../../files/math.py"
    };
    analyzer::metric::MetricExtractor metricExtractor{};

    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;

    MetricsAccumulator accumulator;
};

///
///
///

TEST_F(CategorialAccumulatorGroup, styles) 
{    
    analyzer::AccumulateFunctionAnalysis(analysis, accumulator);
    auto &accStyles =
            accumulator.template GetFinalizedAccumulator<CategoricalAccumulator>(NamingStyleMetric::kName);
    
    auto &styles = accStyles.Get();
    auto it = styles.find("Pascal Case");
    EXPECT_EQ(it->second, 1);
    it = styles.find("Lower Case");
    EXPECT_EQ(it->second, 2);
    it = styles.find("Camel Case");
    EXPECT_EQ(it->second, 1);
    it = styles.find("Snake Case");
    EXPECT_EQ(it->second, 4);
}

///
///
///

TEST_F(CategorialAccumulatorGroup, splitByClassStyles) 
{    
    auto group = analyzer::SplitByClasses(analysis) | std::ranges::to<std::vector>();
    /// sample.py file
    auto firstFile = group.front() | std::ranges::to<std::vector>();
    EXPECT_EQ(firstFile.size(), 3);

    analyzer::AccumulateFunctionAnalysis(firstFile, accumulator);
    auto &accStyles =
            accumulator.template GetFinalizedAccumulator<CategoricalAccumulator>(NamingStyleMetric::kName);
    
    auto &styles = accStyles.Get();
    auto it = styles.find("Lower Case");
    EXPECT_EQ(it->second, 1);
    it = styles.find("Snake Case");
    EXPECT_EQ(it->second, 3);
    
    it = styles.find("Camel Case");
    EXPECT_EQ(it, styles.end());
    it = styles.find("Pascal Case");
    EXPECT_EQ(it, styles.end());
}

}  // namespace analyzer::metric_accumulator::metric_accumulator_impl::test
