// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <any>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui {
/**
 */
class OPENASSETIO_UI_EXPORT UIDelegateState {
 public:
  std::any nativeData;
  EntityReferences entityReferences;
  trait::TraitsDatas entityTraitsDatas;
  trait::TraitsDataPtr relationshipTraitsData;
};
}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
