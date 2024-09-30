// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2024 The Foundry Visionmongers Ltd
#include <exception>
#include <memory>
#include <utility>

#include <fmt/core.h>

#include <openassetio/export.h>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/log/LoggerInterface.hpp>  // NOLINT(*-include-cleaner): needed for logger()
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/UIDelegateState.hpp>
#include <openassetio/ui/access.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::hostApi {

using HostSessionPtr = openassetio::managerApi::HostSessionPtr;

UIDelegate::~UIDelegate() {
  try {
    close();
  } catch (const std::exception& exc) {
    hostSession_->logger()->error(
        fmt::format("Exception in destructor of UIDelegate: {}", exc.what()));
  } catch (...) {
    hostSession_->logger()->error(
        "Exception in destructor of UIDelegate: <unknown non-exception value caught>");
  }
}

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
void UIDelegate::close() { uiDelegateInterface_->close(hostSession_); }

void UIDelegate::flushCaches() { uiDelegateInterface_->flushCaches(hostSession_); }

trait::TraitsDataPtr UIDelegate::uiPolicy(const trait::TraitSet& uiTraits,
                                          access::UIAccess uiAccess,
                                          const ContextConstPtr& context) {
  return uiDelegateInterface_->uiPolicy(uiTraits, uiAccess, context, hostSession_);
}

UIDelegateStateInterfacePtr UIDelegate::populateUI(
    const trait::TraitsDataConstPtr& uiTraitsData, const access::UIAccess uiAccess,
    const UIDelegateRequestInterfacePtr& requestState, const ContextConstPtr& context) {
  return uiDelegateInterface_->populateUI(uiTraitsData, uiAccess, requestState, context,
                                          hostSession_);
}

InfoDictionary UIDelegate::info() { return uiDelegateInterface_->info(); }

}  // namespace ui::hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
