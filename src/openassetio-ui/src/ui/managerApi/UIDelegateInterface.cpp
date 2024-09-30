// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

#include <openassetio/export.h>
#include <openassetio/Context.hpp>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/ui/UIDelegateState.hpp>
#include <openassetio/ui/access.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::managerApi {

UIDelegateInterface::UIDelegateInterface() = default;

InfoDictionary UIDelegateInterface::info() { return {}; }

// NOLINTNEXTLINE(performance-unnecessary-value-param)
void UIDelegateInterface::initialize(InfoDictionary uiDelegateSettings,
                                     [[maybe_unused]] const HostSessionPtr& hostSession) {
  if (!uiDelegateSettings.empty()) {
    throw errors::InputValidationException{
        "Settings provided but are not supported. The initialize method has not been implemented "
        "by the manager."};
  }
}

void UIDelegateInterface::close([[maybe_unused]] const HostSessionPtr& hostSession) {}

InfoDictionary UIDelegateInterface::settings([[maybe_unused]] const HostSessionPtr& hostSession) {
  return {};
}

void UIDelegateInterface::flushCaches([[maybe_unused]] const HostSessionPtr& hostSession) {}

trait::TraitsDataPtr UIDelegateInterface::uiPolicy(
    [[maybe_unused]] const trait::TraitSet& uiTraits,
    [[maybe_unused]] const access::UIAccess uiAccess,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession) {
  return trait::TraitsData::make();
}

UIDelegateStateInterfacePtr UIDelegateInterface::populateUI(
    [[maybe_unused]] const trait::TraitsDataConstPtr& uiTraitsData,
    [[maybe_unused]] const access::UIAccess uiAccess,
    [[maybe_unused]] const UIDelegateRequestInterfacePtr& requestState,
    [[maybe_unused]] const ContextConstPtr& context,
    [[maybe_unused]] const HostSessionPtr& hostSession) {
  return {};
}

}  // namespace ui::managerApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
