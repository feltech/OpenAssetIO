// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#pragma once
#include <vector>

#include "SpecificationData.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

using TraitIds = std::vector<trait::TraitId>;

struct OPENASSETIO_CORE_EXPORT SpecificationBase : HasSpecificationData {
  virtual ~SpecificationBase() = default;

  [[nodiscard]] virtual const TraitIds& traitIDs() const = 0;
};

}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
