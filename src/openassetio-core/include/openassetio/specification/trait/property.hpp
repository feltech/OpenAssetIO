// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Typedefs for the trait property data stored within specifications.
 */
#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>

// API export header.
#include <openassetio/export.h>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification::trait {

namespace property {

/// Boolean value type for specification property dictionaries.
using Bool = bool;
/// Integer value type for specification property dictionaries.
using Int = int;
/// Real value type for specification property dictionaries.
using Float = double;
/// String value type for specification property dictionaries.
using Str = std::string;

/// Property dictionary keys.
using Key = std::string;
/// Property dictionary values.
using Value = std::variant<Bool, Int, Float, Str>;

/**
 * Type used to allow "unset" as a valid return value from specification
 * dictionary queries.
 */
template <class T>
using Maybe = std::optional<T>;
}  // namespace property

/// Trait unique ID type.
using TraitId = property::Key;
}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
