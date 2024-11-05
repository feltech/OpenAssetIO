// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <any>
#include <functional>
#include <utility>

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

class OPENASSETIO_UI_EXPORT UIDelegateRequest {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateRequest)
  using StateChangedCallback = std::function<void(const UIDelegateStateConstPtr&)>;

  static UIDelegateRequestPtr make();
  virtual ~UIDelegateRequest() = default;
  [[nodiscard]] virtual UIDelegateRequestPtr copy() const;

  // Getters
  [[nodiscard]] virtual std::any getNativeData() const { return nativeData_; }
  [[nodiscard]] virtual const EntityReferences& getEntityReferences() const {
    return entityReferences_;
  }
  [[nodiscard]] virtual const trait::TraitsDatas& getEntityTraitsDatas() const {
    return entityTraitsDatas_;
  }
  [[nodiscard]] virtual const StateChangedCallback& getStateChangedCallback() const {
    return stateChangedCallback_;
  }

  // Setters
  virtual void setNativeData(std::any nativeData) { nativeData_ = std::move(nativeData); }
  virtual void setEntityReferences(EntityReferences entityReferences) {
    entityReferences_ = std::move(entityReferences);
  }
  virtual void setEntityTraitsDatas(trait::TraitsDatas entityTraitsDatas) {
    entityTraitsDatas_ = std::move(entityTraitsDatas);
  }
  virtual void setStateChangedCallback(StateChangedCallback callback) {
    stateChangedCallback_ = std::move(callback);
  }

 protected:
  UIDelegateRequest() = default;
  void copyTo(Ptr& other) const;

 private:
  std::any nativeData_;
  EntityReferences entityReferences_;
  trait::TraitsDatas entityTraitsDatas_;
  trait::TraitsDatas relationshipTraitsDatas_;
  StateChangedCallback stateChangedCallback_;
};

class OPENASSETIO_UI_EXPORT UIDelegateState {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateState)
  using UpdateRequestCallback = std::function<void(const UIDelegateRequestConstPtr&)>;

  static UIDelegateStatePtr make();
  virtual ~UIDelegateState() = default;
  [[nodiscard]] virtual UIDelegateStatePtr copy() const;

  // Getters
  [[nodiscard]] virtual std::any getNativeData() const { return nativeData_; }
  [[nodiscard]] virtual const EntityReferences& getEntityReferences() const {
    return entityReferences_;
  }
  [[nodiscard]] virtual const trait::TraitsDatas& getEntityTraitsDatas() const {
    return entityTraitsDatas_;
  }
  [[nodiscard]] virtual const UpdateRequestCallback& getUpdateRequestCallback() const {
    return updateRequestCallback_;
  }

  // Setters
  virtual void setNativeData(std::any nativeData) { nativeData_ = std::move(nativeData); }
  virtual void setEntityReferences(EntityReferences entityReferences) {
    entityReferences_ = std::move(entityReferences);
  }
  virtual void setEntityTraitsDatas(trait::TraitsDatas entityTraitsDatas) {
    entityTraitsDatas_ = std::move(entityTraitsDatas);
  }
  virtual void setUpdateRequestCallback(UpdateRequestCallback callback) {
    updateRequestCallback_ = std::move(callback);
  }

 protected:
  UIDelegateState() = default;
  void copyTo(Ptr& other) const;

 private:
  std::any nativeData_;
  EntityReferences entityReferences_;
  trait::TraitsDatas entityTraitsDatas_;
  UpdateRequestCallback updateRequestCallback_;
};

}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
