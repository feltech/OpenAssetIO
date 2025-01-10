// SPDX-License-Identifier: Apache-2.0
// Copyright 2023-2025 The Foundry Visionmongers Ltd
#include <string_view>
#include <type_traits>
#include <variant>

#include <fmt/args.h>
#include <fmt/base.h>
#include <fmt/format.h>

#include <openassetio/export.h>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/utils/substitute.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace utils {

Str substitute(const std::string_view input, const InfoDictionary& substitutions) {
  fmt::dynamic_format_arg_store<fmt::format_context> args;
  args.reserve(substitutions.size(), substitutions.size());

  for (const auto& substitution : substitutions) {
    // Note: lambda capture of structured binding is not supported in
    // apple-clang on MacOS 11, so we must capture the `pair` and then
    // destructure it within the lambda.
    std::visit(
        [&args, &substitution](const auto& value) {
          const auto& [key, variantValue] = substitution;
          using T = std::decay_t<decltype(value)>;
          if constexpr (std::is_same_v<T, Str>) {
            args.push_back(fmt::arg(key.c_str(), std::string_view{value}));
          } else {
            args.push_back(fmt::arg(key.c_str(), value));
          }
        },
        substitution.second);
  }

  try {
    return vformat(input, args);
  } catch (const fmt::format_error& exc) {
    throw errors::InputValidationException{fmt::format(
        "substitute(): failed to process the input string '{}': {}", input, exc.what())};
  }
}

}  // namespace utils
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
