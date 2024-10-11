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

OPENASSETIO_DECLARE_PTR(UIDelegateRequest)
OPENASSETIO_DECLARE_PTR(UIDelegateState)

/**
 */
class OPENASSETIO_UI_EXPORT UIDelegateRequest {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateRequest)
  std::any nativeData;
  EntityReferences entityReferences;
  trait::TraitsDatas entityTraitsDatas;
  trait::TraitsDatas relationshipTraitsDatas;
  using StateChangedCallback = std::function<void(const UIDelegateStateConstPtr&)>;
  StateChangedCallback stateChangedCallback;

  static UIDelegateRequestPtr make();
  static UIDelegateRequestPtr make(const UIDelegateRequestConstPtr& other);
  static UIDelegateRequestPtr make(std::any nativeData, EntityReferences entityReferences,
                                   trait::TraitsDatas entityTraitsDatas,
                                   trait::TraitsDatas relationshipTraitsDatas,
                                   StateChangedCallback stateChangedCallback);

 private:
  UIDelegateRequest() = default;
  UIDelegateRequest(std::any nativeData, EntityReferences entityReferences,
                    trait::TraitsDatas entityTraitsDatas,
                    trait::TraitsDatas relationshipTraitsData,
                    StateChangedCallback stateChangedCallback);
};

/**
 */
class OPENASSETIO_UI_EXPORT UIDelegateState {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateState)
  std::any nativeData;
  EntityReferences entityReferences;
  trait::TraitsDatas entityTraitsDatas;
  using UpdateRequestCallback = std::function<void(const UIDelegateRequestConstPtr&)>;
  UpdateRequestCallback updateRequestCallback;

  static UIDelegateStatePtr make();
  static UIDelegateStatePtr make(const UIDelegateStateConstPtr& other);
  static UIDelegateStatePtr make(std::any nativeData, EntityReferences entityReferences,
                                 trait::TraitsDatas entityTraitsDatas,
                                 UpdateRequestCallback updateRequestCallback);

 private:
  UIDelegateState() = default;
  UIDelegateState(std::any nativeData, EntityReferences entityReferences,
                  trait::TraitsDatas entityTraitsDatas,
                  UpdateRequestCallback updateRequestCallback);
};
}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
