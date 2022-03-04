// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <openassetio/specification/BlobSpecification.hpp>
#include <openassetio/specification/trait/BlobTrait.hpp>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification {

const TraitIds BlobSpecification::kTraitIds{{trait::BlobTrait::kId}};

const TraitIds& BlobSpecification::traitIDs() const { return kTraitIds; }

}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
