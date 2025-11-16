#include "metric_impl/code_lines_count.hpp"
#include "analyse.hpp"

#include <gtest/gtest.h>

namespace analyzer::metric::metric_impl {

class CodeLinesCountGroup : public testing::Test {
protected:
    void SetUp() override {
        testing::Test::SetUp();
        using namespace analyzer::metric::metric_impl;
        metricExtractor.RegisterMetric(std::make_unique<CodeLinesCountMetric>());
    }

    std::vector<std::string> files = { 
        "../../../src/metric_impl/tests/files/comments.py",
        "../../../src/metric_impl/tests/files/simple.py"
    };
    analyzer::metric::MetricExtractor metricExtractor{};
};

///
///
///

TEST_F(CodeLinesCountGroup, comments) {
    /// Check only a file with comments
    std::vector<std::string> commentedFiles{};
    commentedFiles.push_back(files.front());

    /// See if such a file exists
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(commentedFiles, metricExtractor);
    });
    
    EXPECT_EQ(1, analysis.size());

    auto &[function, result] = analysis.front();
    EXPECT_EQ(function.name, "Func_comments");
    EXPECT_EQ(result[0].metric_name, CodeLinesCountMetric::kName);
    EXPECT_EQ(std::get<int>(result[0].value), 3);
}

///
///
///

TEST_F(CodeLinesCountGroup, commentsSeveralFiles) {
    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(files, metricExtractor);
    });
    
    EXPECT_EQ(2, analysis.size());

    auto values = analysis | std::views::values | std::views::join;
    auto codeLines = std::ranges::fold_left(values, 0, [](int init, const auto &metric){
        return init + std::get<int>(metric.value);;
    });

    EXPECT_EQ(8, codeLines);
}

///
///
///

TEST_F(CodeLinesCountGroup, splitByFiles) {
    std::vector<std::string> f = { 
        "../../../files/sample.py",
        "../../../files/math.py"
    };

    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(f, metricExtractor);
    });
    
    EXPECT_EQ(8, analysis.size());

    /// Get a range of subranges
    auto functionInFiles = analyzer::SplitByFiles(analysis);
    EXPECT_EQ(2, std::ranges::distance(functionInFiles));
    auto functionsInFilesVector = functionInFiles | std::ranges::to<std::vector>();
    EXPECT_EQ(2, functionsInFilesVector.size());

    auto &file_1 = functionsInFilesVector.front();
    auto &file_2 = functionsInFilesVector.back();
    EXPECT_EQ(4, file_1.size());
    EXPECT_EQ(4, file_2.size());

    auto functions_1 = file_1 | std::views::keys;
    EXPECT_TRUE(std::ranges::all_of(functions_1, [](const auto &function){
        return function.filename == "../../../files/sample.py";
    }));

    auto functions_2 = file_2 | std::views::keys;
    EXPECT_TRUE(std::ranges::all_of(functions_2, [](const auto &function){
        return function.filename == "../../../files/math.py";
    }));
}

///
///
///

TEST_F(CodeLinesCountGroup, spriteByClasses) {
    std::vector<std::string> f = { 
        "../../../files/sample.py",
        "../../../files/math.py"
    };

    std::vector<std::pair<function::Function, metric::MetricResults>> analysis;
    EXPECT_NO_THROW({
        analysis = analyzer::AnalyseFunctions(f, metricExtractor);
    });
    EXPECT_EQ(8, analysis.size());

    auto functionInClasses = analyzer::SplitByClasses(analysis);
    EXPECT_EQ(2, std::ranges::distance(functionInClasses));
    auto functionsInClassesVector = functionInClasses | std::ranges::to<std::vector>();
    EXPECT_EQ(2, functionsInClassesVector.size());

    auto &file_1 = functionsInClassesVector.front();
    auto &file_2 = functionsInClassesVector.back();

    auto t = file_1 | std::ranges::to<std::vector>();
    auto p = file_2 | std::ranges::to<std::vector>();
    EXPECT_EQ(3,  std::ranges::distance(file_1));
    EXPECT_EQ(4,  std::ranges::distance(file_2));

    auto functions_1 = file_1 | std::views::keys;
    EXPECT_TRUE(std::ranges::all_of(functions_1, [](const auto &function){
        return function.filename == "../../../files/sample.py";
    }));

    auto functions_2 = file_2 | std::views::keys;
    EXPECT_TRUE(std::ranges::all_of(functions_2, [](const auto &function){
        return function.filename == "../../../files/math.py";
    }));
}

}  // namespace analyzer::metric::metric_impl
