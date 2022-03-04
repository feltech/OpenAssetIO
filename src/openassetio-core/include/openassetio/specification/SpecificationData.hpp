// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#pragma once

#include <memory>
#include <unordered_map>

// API export generated header
#include <openassetio/export.h>

#include "trait/property.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

class OPENASSETIO_CORE_EXPORT SpecificationData {
 public:
  SpecificationData() = default;

  [[nodiscard]] std::size_t size() const;

  template <class T>
  [[nodiscard]] trait::property::Maybe<T> getTraitProperty(const trait::TraitId traitId,
                                                    const trait::property::Key& propertyKey) {
    const auto traitIter = traitDict_.find(traitId);
    if (traitIter == traitDict_.end()) {
      return {};
    }

    const trait::Properties& props = traitIter->second;
    const auto propIter = props.find(propertyKey);
    if (propIter == props.end()) {
      return {};
    }

    // TODO(DF): Make exception message more friendly.
    return std::get<T>(propIter->second);
  }

  void setTraitProperty(trait::TraitId traitId, const trait::property::Key& propertyKey,
                        trait::property::Value propertyValue);

 private:
  using TraitDict = std::unordered_map<trait::TraitId, trait::Properties>;
  TraitDict traitDict_;
};

using SpecificationDataPtr = std::shared_ptr<SpecificationData>;

class HasSpecificationData {
 public:
  HasSpecificationData() = default;
  explicit HasSpecificationData(SpecificationDataPtr data);
  [[nodiscard]] const SpecificationDataPtr& data() const;
  SpecificationDataPtr& data();

 private:
  SpecificationDataPtr data_;
};
}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
