// Copyright 4- The Foundry Visionmongers Ltd
#include <openassetio/pluginSystem/CppPluginSystemManagerImplementationFactory.hpp>

#include <cstdlib>
#include <memory>
#include <unordered_map>
#include <utility>

#include <fmt/format.h>

#include <openassetio/errors/exceptions.hpp>
#include <openassetio/log/LoggerInterface.hpp>
#include <openassetio/pluginSystem/CppPluginSystem.hpp>
#include <openassetio/pluginSystem/CppPluginSystemManagerPlugin.hpp>
#include <openassetio/typedefs.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace pluginSystem {

namespace {
/**
 * Helper function to retrieve plugin search paths.
 *
 * @param logger Logger to warn if no paths available.
 * @param candidatePaths Fixed paths to use else fallback to env var.
 * @return Either given paths or paths from env var.
 */
Str paths(const log::LoggerInterfacePtr& logger, Str candidatePaths) {
  if (candidatePaths.empty()) {
    const char* envVar = CppPluginSystemManagerImplementationFactory::kPluginEnvVar.data();
    if (const char* envPaths = std::getenv(envVar)) {
      candidatePaths = envPaths;
    }
  }
  if (candidatePaths.empty()) {
    logger->log(log::LoggerInterface::Severity::kWarning,
                fmt::format("No search paths specified, no plugins will load - check ${} is set",
                            CppPluginSystemManagerImplementationFactory::kPluginEnvVar));
  }
  return candidatePaths;
}
}  // namespace

CppPluginSystemManagerImplementationFactoryPtr CppPluginSystemManagerImplementationFactory::make(
    openassetio::Str paths, log::LoggerInterfacePtr logger) {
  return std::make_shared<CppPluginSystemManagerImplementationFactory>(
      CppPluginSystemManagerImplementationFactory{std::move(paths), std::move(logger)});
}

CppPluginSystemManagerImplementationFactoryPtr CppPluginSystemManagerImplementationFactory::make(
    log::LoggerInterfacePtr logger) {
  return std::make_shared<CppPluginSystemManagerImplementationFactory>(
      CppPluginSystemManagerImplementationFactory{std::move(logger)});
}

CppPluginSystemManagerImplementationFactory::CppPluginSystemManagerImplementationFactory(
    openassetio::Str fixedPaths, log::LoggerInterfacePtr logger)
    : ManagerImplementationFactoryInterface{std::move(logger)},
      fixedPaths_{std::move(fixedPaths)} {}

CppPluginSystemManagerImplementationFactory::CppPluginSystemManagerImplementationFactory(
    log::LoggerInterfacePtr logger)
    : CppPluginSystemManagerImplementationFactory{
          // Empty paths initially - will be filled lazily from
          // kPluginEnvVar.
          {},
          // getenv returns nullptr if var not set, which cannot be
          // used to construct a std::string.
          std::move(logger)} {}

Identifiers CppPluginSystemManagerImplementationFactory::identifiers() {
  if (!pluginSystem_) {
    // Lazy load plugins.
    pluginSystem_ = CppPluginSystem::make(logger());
    pluginSystem_->scan(paths(logger(), fixedPaths_));
  }

  // Get all OpenAssetIO plugins, whether manager plugins or otherwise.
  openassetio::Identifiers pluginIds = pluginSystem_->identifiers();

  // Filter plugins to only those that are manager plugins.
  pluginIds.erase(
      std::remove_if(
          begin(pluginIds), end(pluginIds),
          [&](const auto& identifier) {
            const auto& [path, plugin] = pluginSystem_->plugin(identifier);

            auto managerPlugin = std::dynamic_pointer_cast<CppPluginSystemManagerPlugin>(plugin);

            if (!managerPlugin) {
              logger()->log(
                  log::LoggerInterface::Severity::kWarning,
                  fmt::format(
                      "Plugin '{}' from '{}' is not a manager plugin as it cannot be cast to a"
                      " CppPluginSystemManagerPlugin",
                      identifier, path.string()));
            }

            return !managerPlugin;
          }),
      end(pluginIds));
  return pluginIds;
}

managerApi::ManagerInterfacePtr CppPluginSystemManagerImplementationFactory::instantiate(
    const Identifier& identifier) {
  if (!pluginSystem_) {
    // Lazy load plugins.
    pluginSystem_ = CppPluginSystem::make(logger());
    pluginSystem_->scan(paths(logger(), fixedPaths_));
  }
  const auto& [path, plugin] = pluginSystem_->plugin(identifier);

  auto managerPlugin = std::dynamic_pointer_cast<CppPluginSystemManagerPlugin>(plugin);

  if (!managerPlugin) {
    throw errors::InputValidationException{
        fmt::format("Plugin '{}' from '{}' is not a manager plugin as it cannot be cast to a"
                    " CppPluginSystemManagerPlugin",
                    identifier, path.string())};
  }

  return managerPlugin->interface();
}
}  // namespace pluginSystem
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
