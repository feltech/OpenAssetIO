// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2024 The Foundry Visionmongers Ltd
#include <memory>
#include <optional>
#include <utility>

#include <openassetio/export.h>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/UIDelegateState.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::hostApi {

using HostSessionPtr = openassetio::managerApi::HostSessionPtr;

UIDelegatePtr UIDelegate::make(managerApi::UIDelegateInterfacePtr uiDelegateInterface,
                               HostSessionPtr hostSession) {
  return std::shared_ptr<UIDelegate>(
      new UIDelegate(std::move(uiDelegateInterface), std::move(hostSession)));
}

UIDelegate::UIDelegate(managerApi::UIDelegateInterfacePtr uiDelegateInterface,
                       HostSessionPtr hostSession)
    : uiDelegateInterface_{std::move(uiDelegateInterface)}, hostSession_{std::move(hostSession)} {}

Identifier UIDelegate::identifier() const { return uiDelegateInterface_->identifier(); }

Str UIDelegate::displayName() const { return uiDelegateInterface_->displayName(); }

InfoDictionary UIDelegate::settings() { return uiDelegateInterface_->settings(hostSession_); }

void UIDelegate::initialize(InfoDictionary uiDelegateSettings) {
  uiDelegateInterface_->initialize(std::move(uiDelegateSettings), hostSession_);
}

void UIDelegate::flushCaches() { uiDelegateInterface_->flushCaches(hostSession_); }

std::optional<UIDelegateState> UIDelegate::populateUI(
    const trait::TraitsDataConstPtr& uiTraitsData, const UIDelegateRequest& requestState,
    const ContextConstPtr& context) {
  return uiDelegateInterface_->populateUI(uiTraitsData, requestState, context, hostSession_);
}

InfoDictionary UIDelegate::info() { return uiDelegateInterface_->info(); }

}  // namespace ui::hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
