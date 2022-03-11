// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Provides the BlobSpecification core specification type.
 */
#pragma once
#include <openassetio/export.h>

#include "Specification.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

/**
 * Core specification for a locatable blob of data.
 *
 * Composes the "blob" trait.
 */
struct OPENASSETIO_CORE_EXPORT BlobSpecification : Specification {
  /**
   * Construct as a Specification supporting the "blob" trait.
   */
  BlobSpecification();
};
}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
