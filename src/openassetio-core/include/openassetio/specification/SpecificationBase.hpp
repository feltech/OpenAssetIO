// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#pragma once
#include <vector>

#include "SpecificationData.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

/// List of trait IDs that a specification supports.
using TraitIds = std::vector<trait::TraitId>;

/**
 * Abstract base class of all specifications.
 */
struct OPENASSETIO_CORE_EXPORT SpecificationBase : HasSpecificationData {
  [[nodiscard]] virtual const TraitIds& traitIDs() const = 0;
};

}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
