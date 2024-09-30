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

class UIDelegateStateInterface;

OPENASSETIO_DECLARE_PTR(UIDelegateRequestInterface)
OPENASSETIO_DECLARE_PTR(UIDelegateStateInterface)

class OPENASSETIO_UI_EXPORT UIDelegateRequestInterface {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateRequestInterface)
  using StateChangedCallback = std::function<void(const UIDelegateStateInterfacePtr&)>;

  virtual ~UIDelegateRequestInterface() = default;

  [[nodiscard]] virtual std::any nativeData() = 0;
  [[nodiscard]] virtual EntityReferences entityReferences() = 0;
  [[nodiscard]] virtual trait::TraitsDatas entityTraitsDatas() = 0;
  [[nodiscard]] virtual StateChangedCallback stateChangedCallback() = 0;
};

class OPENASSETIO_UI_EXPORT UIDelegateStateInterface {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateStateInterface)
  using UpdateRequestCallback = std::function<void(const UIDelegateRequestInterfacePtr&)>;

  virtual ~UIDelegateStateInterface() = default;

  // Getters
  [[nodiscard]] virtual std::any nativeData() = 0;
  [[nodiscard]] virtual EntityReferences entityReferences() = 0;
  [[nodiscard]] virtual trait::TraitsDatas entityTraitsDatas() = 0;
  [[nodiscard]] virtual UpdateRequestCallback updateRequestCallback() = 0;
};

}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
