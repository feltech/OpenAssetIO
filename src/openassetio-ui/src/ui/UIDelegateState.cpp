// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <openassetio/ui/UIDelegateState.hpp>

#include <any>
#include <memory>
#include <utility>

#include <openassetio/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/collection.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui {
UIDelegateRequest::UIDelegateRequest(std::any nativeData_, EntityReferences entityReferences_,
                                     trait::TraitsDatas entityTraitsDatas_,
                                     trait::TraitsDatas relationshipTraitsDatas_,
                                     StateChangedCallback stateChangedCallback_)
    : nativeData{std::move(nativeData_)},
      entityReferences{std::move(entityReferences_)},
      entityTraitsDatas{std::move(entityTraitsDatas_)},
      relationshipTraitsDatas{std::move(relationshipTraitsDatas_)},
      stateChangedCallback{std::move(stateChangedCallback_)} {};

UIDelegateRequestPtr UIDelegateRequest::make() {
  return std::make_shared<UIDelegateRequest>(UIDelegateRequest{});
}

UIDelegateRequestPtr UIDelegateRequest::make(std::any nativeData,
                                             EntityReferences entityReferences,
                                             trait::TraitsDatas entityTraitsDatas,
                                             trait::TraitsDatas relationshipTraitsDatas,
                                             StateChangedCallback stateChangedCallback) {
  return std::make_shared<UIDelegateRequest>(UIDelegateRequest{
      std::move(nativeData), std::move(entityReferences), std::move(entityTraitsDatas),
      std::move(relationshipTraitsDatas), std::move(stateChangedCallback)});
}

UIDelegateState::UIDelegateState(std::any nativeData_, EntityReferences entityReferences_,
                                 trait::TraitsDatas entityTraitsDatas_,
                                 UpdateRequestCallback updateRequestCallback_)
    : nativeData{std::move(nativeData_)},
      entityReferences{std::move(entityReferences_)},
      entityTraitsDatas{std::move(entityTraitsDatas_)},
      updateRequestCallback{std::move(updateRequestCallback_)} {}

UIDelegateStatePtr UIDelegateState::make() {
  return std::make_shared<UIDelegateState>(UIDelegateState{});
}

UIDelegateStatePtr UIDelegateState::make(std::any nativeData, EntityReferences entityReferences,
                                         trait::TraitsDatas entityTraitsDatas,
                                         UpdateRequestCallback updateRequestCallback) {
  return std::make_shared<UIDelegateState>(
      UIDelegateState{std::move(nativeData), std::move(entityReferences),
                      std::move(entityTraitsDatas), std::move(updateRequestCallback)});
}
}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
