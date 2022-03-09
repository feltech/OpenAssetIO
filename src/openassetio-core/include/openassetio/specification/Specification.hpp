// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Provide the base dynamic specification class.
 */
#pragma once

#include <memory>
#include <vector>

#include "trait/property.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

/**
 * Base specification class that is defined by a given list of trait
 * IDs.
 */
class OPENASSETIO_CORE_EXPORT Specification {
 public:
  using TraitIds = std::vector<trait::TraitId>;

  explicit Specification(const TraitIds& traitIds);
  virtual ~Specification();

  [[nodiscard]] bool hasTrait(const trait::TraitId& traitId) const;

  [[nodiscard]] trait::property::Maybe<trait::property::Value> getTraitProperty(
      const trait::TraitId& traitId, const trait::property::Key& propertyKey) const;

  void setTraitProperty(const trait::TraitId& traitId, const trait::property::Key& propertyKey,
                        trait::property::Value propertyValue);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
