// SPDX-License-Identifier: Apache-2.0
// Copyright 2023-2025 The Foundry Visionmongers Ltd
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
 * Constants used throughout the OpenAssetIO API.
 */
namespace constants {

/**
 * @name Info dictionary field names.
 *
 * Bare strings should never be used to help protect against
 * inconsistencies and future changes.
 *
 * @{
 */

// General

// NOLINTBEGIN(readability-identifier-naming): requires source breaking change.

// Entity Reference Properties

/**
 * Common prefix for all entity references of a particular manager.
 *
 * This field may be used by the API to optimize queries to
 * isEntityReferenceString in situations where bridging languages, etc.
 * can be expensive (particularly in the case of python plug-ins called
 * from multi-threaded C++).
 */
inline constexpr std::string_view kInfoKey_EntityReferencesMatchPrefix =
    "entityReferencesMatchPrefix";

// NOLINTEND(readability-identifier-naming)
/// @}
}  // namespace constants
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
