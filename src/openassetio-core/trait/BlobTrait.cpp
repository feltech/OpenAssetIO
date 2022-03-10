// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <algorithm>
#include <utility>

#include <openassetio/trait/BlobTrait.hpp>
#include <openassetio/trait/property.hpp>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace trait {

namespace {
const property::Key kUrl = "url";
const property::Key kMimeType = "mimeType";
}  // namespace

bool BlobTrait::getUrl(property::Str* out) const {
  return getTraitProperty<property::Str>(out, kId, kUrl);
}

void BlobTrait::setUrl(property::Str url) { spec()->setTraitProperty(kId, kUrl, std::move(url)); }

bool BlobTrait::getMimeType(property::Str* out) const {
  return getTraitProperty<property::Str>(out, kId, kMimeType);
}

void BlobTrait::setMimeType(property::Str mimeType) {
  spec()->setTraitProperty(kId, kMimeType, std::move(mimeType));
}

}  // namespace trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
