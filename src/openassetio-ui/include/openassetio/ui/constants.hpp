// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once
/**
 * @file
 *
 * Defines common keys for lookup in manager information dictionaries.
 */
#include <string_view>

#include <openassetio/export.h>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
/**
 * Constants used throughout the OpenAssetIO UI delegation API.
 */
namespace ui::constants {

/**
 * @name Info dictionary field names.
 *
 * Bare strings should never be used to help protect against
 * inconsistencies and future changes.
 *
 * @{
 */

// NOLINTBEGIN(readability-identifier-naming): requires source breaking change.

inline constexpr std::string_view kInfoKey_SmallIcon = "smallIcon";
inline constexpr std::string_view kInfoKey_Icon = "icon";

/**
 */
inline constexpr std::string_view kInfoKey_PythonOnly = "pythonOnly";

// NOLINTEND(readability-identifier-naming)
/// @}
}  // namespace ui::constants
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
