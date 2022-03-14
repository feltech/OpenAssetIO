// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Convenience statically defined property key constants.
 */
#pragma once

#include <openassetio/trait/property.hpp>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace trait {
/// Key for a URL property.
inline const property::Key kUrl{"url"};
/// Key for a mime type property.
inline const property::Key kMimeType{"mimeType"};
}  // namespace trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
