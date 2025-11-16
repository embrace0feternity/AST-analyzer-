#include <unistd.h>
#include <iterator>

#include "file.hpp"
#include "function.hpp"
#include "metric.hpp"
#include "metric_accumulator.hpp"

#include <print>

namespace analyzer {

namespace rv = std::ranges::views;
namespace rs = std::ranges;
/**
 * @brief Анализирует список Python-файлов и извлекает метрики для всех функций и методов.
 *
 * Эта функция — центральный "конвейер" обработки:
 * 1. Принимает имена файлов.
 * 2. Для каждого файла создаёт объект `File`, который автоматически парсит его через tree-sitter
 *    и строит AST.
 * 3. Извлекает из AST все функции и методы с помощью `FunctionExtractor`.
 * 4. Объединяет все функции из всех файлов в один плоский список (`join`).
 * 5. Для каждой функции вычисляет набор метрик через переданный `metric_extractor`.
 * 6. Возвращает вектор пар: (функция, результаты её метрик).
 */
inline auto AnalyseFunctions(const std::vector<std::string> &files,
                      const metric::MetricExtractor &metric_extractor) {
    /// Не понял, как 4й пункт связан с 6. Поэтому сделал вот так
    std::vector<std::pair<function::Function, metric::MetricResults>> metrics;

    std::ranges::for_each(files, [&metrics, &metric_extractor](const std::string &fileName){
        file::File file { fileName };
        auto functions = function::FunctionExtractor{}.Get(file);

        std::ranges::transform(functions, std::back_inserter(metrics), 
            [&metric_extractor](const function::Function &function){
                return std::make_pair(function, metric_extractor.Get(function));
            }
        );
    });

    return metrics;
}

/**
 * 
 * @brief Группирует результаты анализа по классам.
 *
 * Эта функция:
 * 1. Отфильтровывает только те функции, которые являются **методами классов**
 *    (у них `class_name.has_value()` == true).
 * 2. Группирует последовательные элементы с одинаковым именем класса с помощью `chunk_by`.
 *
 * Важно:
 * - `chunk_by` работает только с **последовательными** одинаковыми элементами!
 *   Поэтому предполагается, что входной диапазон уже упорядочен по классам
 *   (например, порядок методов в AST сохраняется как в исходном файле).
 * - Если порядок нарушен, один и тот же класс может быть разбит на несколько групп.
 *
 *  Чтобы убедиться, что фильтрация работает, проверьте, что свободные функции (без class_name)
 * действительно исчезают из результата.
 */
auto SplitByClasses(const auto &analysis) {
    auto onlyClassMethods = [](const auto &analysisResult){
        return static_cast<bool>(analysisResult.first.class_name);
    };

    return analysis | 
            std::views::filter(onlyClassMethods) |
            std::views::chunk_by(
                [](const auto &analysisResultThs, const auto &analysisResultOther){
        return analysisResultThs.first.filename == analysisResultOther.first.filename;
    });
}

/**
 * @brief Группирует результаты анализа по исходным файлам.
 *
 * Эта функция:
 * - Разбивает весь список функций на группы, где каждая группа содержит
 *   только функции из одного и того же файла (`filename`).
 * - Использует `chunk_by`, поэтому **порядок функций в `analysis` должен быть по файлам**.
 */
auto SplitByFiles(const auto &analysis) {
    return analysis | std::views::chunk_by([](const auto &analysisResultThs, const auto &analysisResultOther){
        return analysisResultThs.first.filename == analysisResultOther.first.filename;
    });
}

/**
 * @brief Агрегирует метрики всех функций с помощью аккумулятора.
 *
 * Эта функция:
 * - Проходит по каждому элементу результата `AnalyseFunctions`
 *   (то есть по каждой функции и её метрикам).
 * - Передаёт результаты метрик (`elem.second`) в аккумулятор через `AccumulateNextFunctionResults`.
 */
void AccumulateFunctionAnalysis(const auto &analysis,
                                analyzer::metric_accumulator::MetricsAccumulator &accumulator) {  
    auto metricResults = analysis | std::views::values;

    std::ranges::for_each(metricResults, [&accumulator](auto &mr){
        accumulator.AccumulateNextFunctionResults(mr);
    });
}

}  // namespace analyzer
