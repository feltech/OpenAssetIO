// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Define the core `BlobTrait` trait class.
 */
#pragma once

#include <openassetio/export.h>

#include "TraitBase.hpp"
#include "property.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification::trait {

/**
 * Core trait class representing a locatable blob of data of a specific
 * type.
 *
 * Has the ID of "blob" and defines url and mimeType properties.
 */
struct OPENASSETIO_CORE_EXPORT BlobTrait : TraitBase<BlobTrait> {
  static inline const TraitId kId{"blob"};

  using TraitBase<BlobTrait>::TraitBase;

  [[nodiscard]] property::Maybe<property::Str> getUrl() const;
  void setUrl(property::Str url);

  [[nodiscard]] property::Maybe<property::Str> getMimeType() const;
  void setMimeType(property::Str mimeType);
};
}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
