// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#pragma once
#include <algorithm>

#include "../SpecificationBase.hpp"
#include "../SpecificationData.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification::trait {

template <class Derived>
struct OPENASSETIO_CORE_EXPORT TraitBase : HasSpecificationData {
  static constexpr TraitId kId = Derived::kId;

  explicit TraitBase(SpecificationDataPtr specificationData)
      : HasSpecificationData(specificationData) {}

  explicit TraitBase(const SpecificationBase& spec)
      : HasSpecificationData(specDataForTrait(spec)) {}

  virtual ~TraitBase() = default;

  [[nodiscard]] TraitId traitId() const { return kId; }
  [[nodiscard]] bool isValid() const { return data().get() != nullptr; }

 private:
  [[nodiscard]] SpecificationDataPtr specDataForTrait(const SpecificationBase& spec) const {
    const auto& specTraitIDs = spec.traitIDs();
    if (std::find(specTraitIDs.begin(), specTraitIDs.end(), kId) != specTraitIDs.end()) {
      return spec.data();
    }
    return {};
  }
};
}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
