// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#pragma once
#include <openassetio/export.h>

#include "SpecificationBase.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

struct OPENASSETIO_CORE_EXPORT BlobSpecification : SpecificationBase {
  static const TraitIds kTraitIds;

  [[nodiscard]] const TraitIds& traitIDs() const override;
};
}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
