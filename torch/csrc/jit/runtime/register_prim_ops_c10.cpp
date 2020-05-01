#include <ATen/ATen.h>
#include <ATen/core/op_registration/op_registration.h>
#include <ATen/core/stack.h>
#include <c10/util/string_utils.h>

using Stack = std::vector<c10::IValue>;
using at::Scalar;
using at::Tensor;
using c10::IValue;
using torch::jit::drop;
using torch::jit::pack;
using torch::jit::peek;
using torch::jit::pop;
using torch::jit::push;

static auto registry_prim =
    torch::RegisterOperators()
        .op("aten::Int.Tensor(Tensor a) -> int",
            torch::RegisterOperators::options()
                .catchAllKernel(
                    [](at::Tensor a) -> int64_t { return a.item<int64_t>(); })
                .aliasAnalysis(c10::AliasAnalysisKind::FROM_SCHEMA))
        .op("aten::Int.bool(bool a) -> int",
            torch::RegisterOperators::options()
                .catchAllKernel(
                    [](bool b) -> int64_t { return static_cast<int64_t>(b); })
                .aliasAnalysis(c10::AliasAnalysisKind::FROM_SCHEMA))
        .op("aten::Int.float(float a) -> int",
            torch::RegisterOperators::options()
                .catchAllKernel(
                    [](double d) -> int64_t { return static_cast<int64_t>(d); })
                .aliasAnalysis(c10::AliasAnalysisKind::FROM_SCHEMA))
        .op("aten::Int.Scalar(Scalar a) -> int",
            torch::RegisterOperators::options()
                .catchAllKernel(
                    [](Scalar scalar) -> int64_t { return scalar.toInt(); })
                .aliasAnalysis(c10::AliasAnalysisKind::FROM_SCHEMA))
        .op("aten::Int.str(str a) -> int",
            torch::RegisterOperators::options()
                .catchAllKernel([](const std::string& str) -> int64_t {
                  std::string::size_type sz;
                  int64_t val = static_cast<int64_t>(c10::stoll(str, &sz));
                  if (sz != str.size()) {
                    std::stringstream error_str;
                    error_str << "invalid literal for int() "
                              << "with base 10: '" << str << "'";
                    throw std::runtime_error(error_str.str());
                  }
                  return val;
                })
                .aliasAnalysis(c10::AliasAnalysisKind::FROM_SCHEMA));
