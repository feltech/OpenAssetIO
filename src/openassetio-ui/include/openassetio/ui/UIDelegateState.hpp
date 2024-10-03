// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <any>
#include <functional>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>

OPENASSETIO_FWD_DECLARE(TraitsData)

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui {

class UIDelegateRequest;
class UIDelegateState;

/**
 */
class OPENASSETIO_UI_EXPORT UIDelegateRequest {
 public:
  std::any nativeData;
  EntityReferences entityReferences;
  trait::TraitsDatas entityTraitsDatas;
  trait::TraitsDataPtr relationshipTraitsData;
  using StateChangedCallback = std::function<void(const UIDelegateState&)>;
  StateChangedCallback stateChangedCallback;
};

/**
 */
class OPENASSETIO_UI_EXPORT UIDelegateState {
 public:
  std::any nativeData;
  EntityReferences entityReferences;
  trait::TraitsDatas entityTraitsDatas;
  using UpdateRequestCallback = std::function<void(const UIDelegateRequest&)>;
  UpdateRequestCallback updateRequestCallback;
};
}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
