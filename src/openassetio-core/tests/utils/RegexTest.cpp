// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#include <fmt/format.h>
#include <catch2/catch.hpp>

#include <openassetio/errors/exceptions.hpp>

#include <utils/Regex.hpp>

// Note pathTo/FromUrl tests cover usage, here we're just testing error
// cases.

using Catch::Matchers::Exception::ExceptionMessageMatcher;
using openassetio::errors::InputValidationException;
using openassetio::utils::Regex;

TEST_CASE("Invalid pattern exception") {
  CHECK_THROWS_MATCHES(
      Regex{"("}, InputValidationException,
      ExceptionMessageMatcher{"Error 114 compiling regex '(': missing closing parenthesis"});
}

TEST_CASE("Invalid JIT pattern exception") {
  // From PCRE2 docs:
  // > There is a limit to the size of pattern that JIT supports,
  //   imposed by the size of machine stack that it uses. The exact
  //   rules are not documented because they may change at any time
  constexpr std::size_t kMaxSingleDotPatterns = 2727;  // Experimentally determined.
  std::string longPattern;
  for (std::size_t patternIdx = 0; patternIdx < kMaxSingleDotPatterns + 1; patternIdx++) {
    longPattern += "(.)";
  }
  CHECK_THROWS_MATCHES(Regex{longPattern}, InputValidationException,
                       ExceptionMessageMatcher{fmt::format(
                           "Error -48 JIT compiling '{}': no more memory", longPattern)});
}

TEST_CASE("Invalid match exception") {
  Regex regex{"(*LIMIT_MATCH=1)((a+)b)+"};
  CHECK_THROWS_MATCHES(
      regex.match("abab"), InputValidationException,
      ExceptionMessageMatcher{"Error -47 matching regex to 'abab': match limit exceeded"});
}

TEST_CASE("Invalid substitution exception") {
  Regex regex{"a"};
  CHECK_THROWS_MATCHES(
      regex.substituteToReduceSize("a", "aa"), InputValidationException,
      ExceptionMessageMatcher{
          "Error -48 substituting regex matches in 'a' with 'aa': no more memory"});
}
