// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <openassetio/specification/trait/BlobTrait.hpp>
// System headers
#include <algorithm>
#include <utility>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification::trait {

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

}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
