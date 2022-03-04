// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <algorithm>
#include <openassetio/specifications.hpp>
namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace traits {

std::size_t SpecificationData::size() const { return traitDict_.size(); }

const Properties& SpecificationData::at(const property::Key& key) const {
  return traitDict_.at(key);
}

Properties& SpecificationData::at(const property::Key& key) { return traitDict_.at(key); }

void SpecificationData::setTraitProperty(const TraitId traitId, const property::Key& propertyKey,
                                         property::Value propertyValue) {
  traitDict_[traitId][propertyKey] = std::move(propertyValue);
}

HasSpecificationData::HasSpecificationData(SpecificationDataPtr data) : data_(std::move(data)) {}

const SpecificationDataPtr& HasSpecificationData::data() const { return data_; }

SpecificationDataPtr& HasSpecificationData::data() { return data_; }

property::Maybe<property::Str> BlobTrait::getUrl() const {
  return data()->getTraitProperty<property::Str>(kId, "url");
}

void BlobTrait::setUrl(property::Str url) { data()->setTraitProperty(kId, "url", std::move(url)); }

[[maybe_unused]] property::Maybe<property::Str> BlobTrait::getMimeType() const {
  return data()->getTraitProperty<property::Str>(kId, "mimeType");
}

void BlobTrait::setMimeType(property::Str mimeType) {
  data()->setTraitProperty(kId, "mimeType", std::move(mimeType));
}

const BaseSpecification::TraitIds BlobSpecification::kTraitIds{{BlobTrait::kId}};

const BaseSpecification::TraitIds& BlobSpecification::traitIDs() const { return kTraitIds; }

}  // namespace traits
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
