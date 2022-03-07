// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
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
using Key = std::string;

using Bool = bool;
using Int = int;
using Float = double;
using Str = std::string;
using Map = std::unordered_map<Key, std::variant<Bool, Int, Float, Str>>;

using Value = std::variant<Bool, Int, Float, Str, Map>;

template <class T>
using Maybe = std::optional<T>;
}  // namespace property

using TraitId = property::Key;
using Properties = std::unordered_map<property::Key, property::Value>;
}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
