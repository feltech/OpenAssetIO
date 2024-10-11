// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <algorithm>
#include <any>
#include <iterator>
#include <memory>
#include <utility>

#include <openassetio/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/ui/UIDelegateState.hpp>

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
UIDelegateRequestPtr UIDelegateRequest::make(const UIDelegateRequestConstPtr& other) {
  trait::TraitsDatas entityTraitsDatas(other->entityTraitsDatas.size());
  trait::TraitsDatas relationshipTraitsDatas(other->relationshipTraitsDatas.size());
  transform(cbegin(other->entityTraitsDatas), cend(other->entityTraitsDatas),
            begin(entityTraitsDatas), [](const trait::TraitsDataConstPtr& traitsData) {
              return trait::TraitsData::make(traitsData);
            });
  transform(cbegin(other->relationshipTraitsDatas), cend(other->relationshipTraitsDatas),
            begin(relationshipTraitsDatas), [](const trait::TraitsDataConstPtr& traitsData) {
              return trait::TraitsData::make(traitsData);
            });

  return make(other->nativeData, other->entityReferences, entityTraitsDatas,
              relationshipTraitsDatas, other->stateChangedCallback);
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

UIDelegateStatePtr UIDelegateState::make(const UIDelegateStateConstPtr& other) {
  trait::TraitsDatas entityTraitsDatas(other->entityTraitsDatas.size());
  transform(cbegin(other->entityTraitsDatas), cend(other->entityTraitsDatas),
            begin(entityTraitsDatas), [](const trait::TraitsDataConstPtr& traitsData) {
              return trait::TraitsData::make(traitsData);
            });

  return make(other->nativeData, other->entityReferences, entityTraitsDatas,
              other->updateRequestCallback);
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
