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

UIDelegateRequestPtr UIDelegateRequest::make() {
  return std::make_shared<UIDelegateRequest>(UIDelegateRequest{});
}

UIDelegateRequestPtr UIDelegateRequest::copy() const {
  auto other = make();
  copyTo(other);
  return other;
}

void UIDelegateRequest::copyTo(Ptr& other) const {
  trait::TraitsDatas entityTraitsDatas(entityTraitsDatas_.size());
  transform(cbegin(entityTraitsDatas_), cend(entityTraitsDatas_), begin(entityTraitsDatas),
            [](const trait::TraitsDataConstPtr& traitsData) {
              return trait::TraitsData::make(traitsData);
            });

  other->entityTraitsDatas_ = std::move(entityTraitsDatas);
  other->nativeData_ = nativeData_;
  other->entityReferences_ = entityReferences_;
  other->stateChangedCallback_ = stateChangedCallback_;
}

UIDelegateStatePtr UIDelegateState::make() {
  return std::make_shared<UIDelegateState>(UIDelegateState{});
}

UIDelegateStatePtr UIDelegateState::copy() const {
  auto other = make();
  copyTo(other);
  return other;
}

void UIDelegateState::copyTo(Ptr& other) const {
  trait::TraitsDatas entityTraitsDatas(entityTraitsDatas_.size());
  transform(cbegin(entityTraitsDatas_), cend(entityTraitsDatas_), begin(entityTraitsDatas),
            [](const trait::TraitsDataConstPtr& traitsData) {
              return trait::TraitsData::make(traitsData);
            });
  other->entityTraitsDatas_ = std::move(entityTraitsDatas);
  other->nativeData_ = nativeData_;
  other->entityReferences_ = entityReferences_;
  other->updateRequestCallback_ = updateRequestCallback_;
}
}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
