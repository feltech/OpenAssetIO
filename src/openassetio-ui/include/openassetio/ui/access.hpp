// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <array>
#include <type_traits>

#include <openassetio/export.h>
#include <openassetio/internal.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
/**
 * Access modes available for API operations.
 */
namespace ui::access {

/**
 * Access pattern requesting a UI element.
 *
 * @see @fqref{ui.hostApi.UIDelegate.populateUI}
 * "UIDelegate.populateUI" /
 * @fqref{ui.managerApi.UIDelegateInterface.populateUI}
 * "UIDelegateInterface.populateUI",
 */
// NOLINTNEXTLINE(performance-enum-size): requires binary breaking change
enum class UIAccess : std::underlying_type_t<internal::access::Access> {
  /**
   * Indicate that the UI state will be used in a read-only operation.
   */
  kRead = internal::access::Access::kRead,

  /**
   * Indicate that the UI state will be used when writing/publishing
   * new data.
   */
  kWrite = internal::access::Access::kWrite,

  /**
   * Indicate that the UI state will be used when writing/publishing
   * new data that is related to, but not the same as, a given entity.
   */
  kCreateRelated = internal::access::Access::kCreateRelated
};
}  // namespace ui::access
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
