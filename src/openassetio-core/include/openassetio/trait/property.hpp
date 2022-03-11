// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Typedefs for the trait property data stored within specifications.
 */
#pragma once

#include <string>
#include <variant>

// API export header.
#include <openassetio/export.h>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace trait {

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
}  // namespace property

/// Trait unique ID type.
using TraitId = property::Key;

/// Status of a trait property within a specification.
enum class TraitPropertyStatus { kFound, kMissing, kInvalidValue };
}  // namespace trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
