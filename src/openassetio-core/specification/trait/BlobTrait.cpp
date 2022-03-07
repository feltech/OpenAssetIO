// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <openassetio/specification/trait/BlobTrait.hpp>
// System headers
#include <algorithm>
#include <utility>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification::trait {

namespace {
const property::Key kUrl = "url";
const property::Key kMimeType = "mimeType";
}  // namespace

property::Maybe<property::Str> BlobTrait::getUrl() const {
  return data()->getTraitProperty<property::Str>(kId, kUrl);
}

void BlobTrait::setUrl(property::Str url) { data()->setTraitProperty(kId, kUrl, std::move(url)); }

[[maybe_unused]] property::Maybe<property::Str> BlobTrait::getMimeType() const {
  return data()->getTraitProperty<property::Str>(kId, kMimeType);
}

void BlobTrait::setMimeType(property::Str mimeType) {
  data()->setTraitProperty(kId, kMimeType, std::move(mimeType));
}

}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
