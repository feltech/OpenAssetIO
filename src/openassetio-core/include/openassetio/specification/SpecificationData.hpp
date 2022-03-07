// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Provides core specification property data storage types.
 */
#pragma once

#include <memory>
#include <unordered_map>

// API export generated header
#include <openassetio/export.h>

#include "trait/property.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

/**
 * Core specification data storage dictionary.
 *
 * All access is via a key pair of trait ID and property key. Queries
 * return optional types, such that if a trait property doesn't exist
 * then the optional evaluates falsey.
 */
class OPENASSETIO_CORE_EXPORT SpecificationData {
 public:
  SpecificationData() = default;

  [[nodiscard]] std::size_t size() const;

  template <class T>
  [[nodiscard]] trait::property::Maybe<T> getTraitProperty(
      const trait::TraitId& traitId, const trait::property::Key& propertyKey) {
    const auto maybeValue = getTraitProperty(traitId, propertyKey);
    if (!maybeValue) {
      return {};
    }
    // TODO(DF): Make exception message more friendly.
    return std::get<T>(*maybeValue);
  }

  [[nodiscard]] trait::property::Maybe<trait::property::Value> getTraitProperty(
      const trait::TraitId& traitId, const trait::property::Key& propertyKey) const;

  void setTraitProperty(const trait::TraitId& traitId, const trait::property::Key& propertyKey,
                        trait::property::Value propertyValue);

 private:
  using Properties = std::unordered_map<trait::property::Key, trait::property::Value>;
  using PropertiesByTrait = std::unordered_map<trait::TraitId, Properties>;
  PropertiesByTrait traitDict_;
};

using SpecificationDataPtr = std::shared_ptr<SpecificationData>;

/**
 * Abstract mixin/base class imbuing a subclass with a SpecificationData
 * member and providing accessors to it.
 */
class HasSpecificationData {
 public:
  [[nodiscard]] const SpecificationDataPtr& data() const;
    SpecificationDataPtr& data();
 protected:
    explicit HasSpecificationData(SpecificationDataPtr data);
    virtual ~HasSpecificationData() = default;
 private:
  SpecificationDataPtr data_;
};
}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
