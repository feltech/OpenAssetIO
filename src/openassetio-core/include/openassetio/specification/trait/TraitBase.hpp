// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Base class for all specification traits.
 */
#pragma once
#include <algorithm>

#include "../SpecificationBase.hpp"
#include "../SpecificationData.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification::trait {

/**
 * Abstract CRTP base class for specification traits.
 *
 * @tparam Derived Concrete subclass.
 */
template <class Derived>
struct OPENASSETIO_CORE_EXPORT TraitBase : HasSpecificationData {
  explicit TraitBase(SpecificationDataPtr specificationData)
      : HasSpecificationData(specificationData) {}

  explicit TraitBase(const SpecificationBase& spec)
      : HasSpecificationData(specDataForTrait(spec)) {}

  [[nodiscard]] static const TraitId& traitId() { return Derived::kId; }
  [[nodiscard]] bool isValid() const { return bool{data()}; }

 private:
  [[nodiscard]] SpecificationDataPtr specDataForTrait(const SpecificationBase& spec) const {
    const auto& specTraitIDs = spec.traitIDs();
    if (std::find(specTraitIDs.begin(), specTraitIDs.end(), traitId()) != specTraitIDs.end()) {
      return spec.data();
    }
    return {};
  }
};
}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
