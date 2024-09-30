// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <openassetio/ui/pluginSystem/CppPluginSystemUIDelegateImplementationFactory.hpp>

#include <algorithm>
#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <utility>

#include <fmt/core.h>
#include <fmt/format.h>

#include <openassetio/export.h>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/log/LoggerInterface.hpp>
#include <openassetio/pluginSystem/CppPluginSystem.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/hostApi/UIDelegateImplementationFactoryInterface.hpp>
#include <openassetio/ui/pluginSystem/CppPluginSystemUIDelegatePlugin.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::pluginSystem {
using CppPluginSystem = openassetio::pluginSystem::CppPluginSystem;

CppPluginSystemUIDelegateImplementationFactoryPtr
CppPluginSystemUIDelegateImplementationFactory::make(Str paths, log::LoggerInterfacePtr logger) {
  return std::make_shared<CppPluginSystemUIDelegateImplementationFactory>(
      CppPluginSystemUIDelegateImplementationFactory{std::move(paths), std::move(logger)});
}

CppPluginSystemUIDelegateImplementationFactoryPtr
CppPluginSystemUIDelegateImplementationFactory::make(log::LoggerInterfacePtr logger) {
  return std::make_shared<CppPluginSystemUIDelegateImplementationFactory>(
      CppPluginSystemUIDelegateImplementationFactory{std::move(logger)});
}

CppPluginSystemUIDelegateImplementationFactory::CppPluginSystemUIDelegateImplementationFactory(
    Str paths, log::LoggerInterfacePtr logger)
    : UIDelegateImplementationFactoryInterface{std::move(logger)}, paths_{std::move(paths)} {
  if (paths_.empty()) {
    this->logger()->log(
        log::LoggerInterface::Severity::kWarning,
        fmt::format("No search paths specified, no plugins will load - check ${} is set",
                    kPluginEnvVar));
  }
}

CppPluginSystemUIDelegateImplementationFactory::CppPluginSystemUIDelegateImplementationFactory(
    log::LoggerInterfacePtr logger)
    : CppPluginSystemUIDelegateImplementationFactory{
          // getenv returns nullptr if var not set, which cannot be
          // used to construct a std::string.
          [paths = std::getenv(kPluginEnvVar.data())] { return paths ? paths : ""; }(),
          std::move(logger)} {}

Identifiers CppPluginSystemUIDelegateImplementationFactory::identifiers() {
  if (!pluginSystem_) {
    // Lazy load plugins.
    pluginSystem_ = CppPluginSystem::make(logger());
    pluginSystem_->scan(paths_);
  }

  // Get all OpenAssetIO plugins, whether manager plugins or otherwise.
  Identifiers pluginIds = pluginSystem_->identifiers();

  // Filter plugins to only those that are manager plugins.
  pluginIds.erase(
      std::remove_if(
          begin(pluginIds), end(pluginIds),
          [&](const auto& identifier) {
            const auto& [path, plugin] = pluginSystem_->plugin(identifier);

            auto uiDelegatePlugin =
                std::dynamic_pointer_cast<CppPluginSystemUIDelegatePlugin>(plugin);

            if (!uiDelegatePlugin) {
              logger()->debug(fmt::format(
                  "Plugin '{}' from '{}' is not a manager plugin as it cannot be cast to a"
                  " CppPluginSystemUIDelegatePlugin",
                  identifier, path.string()));
            }

            return !uiDelegatePlugin;
          }),
      end(pluginIds));
  return pluginIds;
}

managerApi::UIDelegateInterfacePtr CppPluginSystemUIDelegateImplementationFactory::instantiate(
    const Identifier& identifier) {
  if (!pluginSystem_) {
    // Lazy load plugins.
    pluginSystem_ = CppPluginSystem::make(logger());
    pluginSystem_->scan(paths_);
  }
  const auto& [path, plugin] = pluginSystem_->plugin(identifier);

  const auto uiDelegatePlugin = std::dynamic_pointer_cast<CppPluginSystemUIDelegatePlugin>(plugin);

  if (!uiDelegatePlugin) {
    throw errors::InputValidationException{
        fmt::format("Plugin '{}' from '{}' is not a UI delegate plugin as it cannot be"
                    " cast to a CppPluginSystemUIDelegatePlugin",
                    identifier, path.string())};
  }

  return uiDelegatePlugin->interface();
}
}  // namespace ui::pluginSystem
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
