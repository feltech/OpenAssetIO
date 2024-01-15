// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#pragma once
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <openassetio/export.h>
#include <openassetio/typedefs.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace utils {

/**
 * Regular expression compilation, matching and caching.
 *
 * Wraps PCRE2, using its JIT compilation and matching functions.
 *
 * Instances of this class are _not_ thread-safe. Use a separate
 * instance per thread.
 *
 * As well as the regex object itself, matches are cached for subsequent
 * querying.
 */
class Regex {
 public:
  /**
   * Constructor.
   *
   * Pre-compiles the regular expression pattern.
   *
   * Note that:
   * - Patterns are case-insensitive.
   * - `$` matches end of string, not newline.
   * - `.` matches all characters, including newlines.
   *
   * @param pattern Regex pattern.
   */
  explicit Regex(StrView pattern);

  ~Regex();

  /**
   * Check if the regex matches a given subject string.
   *
   * Caches the match results for subsequent retrieval in other methods.
   *
   * @param subject
   *
   * @return `true` if there is a match, `false` otherwise.
   */
  bool match(StrView subject);

  /**
   * Get the string from a group in the last match.
   *
   * Warning: no validation is performed. It is assumed the last match
   * was a success and the given group number exists. It is also assumed
   * that the original subject string is still valid (not been
   * destroyed).
   *
   * @param groupNum Regex capture group to extract.
   *
   * @return Substring view of the match in the original subject string.
   */
  StrView lastMatchGroup(std::size_t groupNum);

  /**
   * Get a new string with all matches of the regex substituted with the
   * given replacement string.
   *
   * @param subject String to copy, with substitutions.
   * @param replacement Replacement to substitute matches with.
   * @return New string that is a copy of @p subject but with matches
   * replaced with @p replacement.
   */
  Str substituteToReduceSize(const StrView& subject, const StrView& replacement);

 private:
  pcre2_code* code_{nullptr};
  pcre2_match_data* matchData_{nullptr};
  struct LastMatch {
    StrView subject;
    int numMatches{0};
  };
  LastMatch lastMatch_;
};
}  // namespace utils
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
