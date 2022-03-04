// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <openassetio/specification/SpecificationData.hpp>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

std::size_t SpecificationData::size() const { return traitDict_.size(); }

void SpecificationData::setTraitProperty(const trait::TraitId traitId,
                                         const trait::property::Key& propertyKey,
                                         trait::property::Value propertyValue) {
  traitDict_[traitId][propertyKey] = std::move(propertyValue);
}

HasSpecificationData::HasSpecificationData(SpecificationDataPtr data) : data_(std::move(data)) {}

const SpecificationDataPtr& HasSpecificationData::data() const { return data_; }

SpecificationDataPtr& HasSpecificationData::data() { return data_; }
}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
