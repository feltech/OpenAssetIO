// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2024 The Foundry Visionmongers Ltd
#include <openassetio/export.h>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

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

InfoDictionary UIDelegateInterface::settings([[maybe_unused]] const HostSessionPtr& hostSession) {
  return {};
}

void UIDelegateInterface::flushCaches([[maybe_unused]] const HostSessionPtr& hostSession) {}

std::any UIDelegateInterface::populateUI(
    [[maybe_unused]] const std::any& container,
    // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    [[maybe_unused]] const trait::TraitsDataConstPtr& uiTraits,
    [[maybe_unused]] const trait::TraitsDataConstPtr& entityTraits,
    [[maybe_unused]] const std::any& nativeData,
    [[maybe_unused]] const HostSessionPtr& hostSession) {
  return {};
}

}  // namespace ui::managerApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio