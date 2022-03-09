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

}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
