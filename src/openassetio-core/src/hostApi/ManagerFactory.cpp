// SPDX-License-Identifier: Apache-2.0
// Copyright 2022-2024 The Foundry Visionmongers Ltd
#if _WIN32
#include <windows.h>  // For SetEnvironmentVariableA
#endif

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <iterator>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <toml++/toml.h>

#include <openassetio/InfoDictionary.hpp>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/hostApi/HostInterface.hpp>
#include <openassetio/hostApi/Manager.hpp>
#include <openassetio/hostApi/ManagerFactory.hpp>
#include <openassetio/hostApi/ManagerImplementationFactoryInterface.hpp>
#include <openassetio/log/LoggerInterface.hpp>
#include <openassetio/managerApi/Host.hpp>
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/managerApi/ManagerInterface.hpp>
#include <openassetio/typedefs.hpp>

#include "../pluginSystem/platform.hpp"

namespace {
constexpr std::string_view kConfigDirVar = "${config_dir}";
/// Environment variable that we assume plugin systems will use to
/// provide their search path.
constexpr std::string_view kPluginEnvVar = "OPENASSETIO_PLUGIN_PATH";
}  // namespace

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace hostApi {

const Str ManagerFactory::kDefaultManagerConfigEnvVarName = "OPENASSETIO_DEFAULT_CONFIG";

ManagerFactoryPtr ManagerFactory::make(
    HostInterfacePtr hostInterface,
    ManagerImplementationFactoryInterfacePtr managerImplementationFactory,
    log::LoggerInterfacePtr logger) {
  return openassetio::hostApi::ManagerFactoryPtr{new ManagerFactory{
      std::move(hostInterface), std::move(managerImplementationFactory), std::move(logger)}};
}

ManagerFactory::ManagerFactory(
    HostInterfacePtr hostInterface,
    ManagerImplementationFactoryInterfacePtr managerImplementationFactory,
    log::LoggerInterfacePtr logger)
    : hostInterface_{std::move(hostInterface)},
      managerImplementationFactory_{std::move(managerImplementationFactory)},
      logger_{std::move(logger)} {}

Identifiers ManagerFactory::identifiers() const {
  return managerImplementationFactory_->identifiers();
}

ManagerFactory::ManagerDetails ManagerFactory::availableManagers() const {
  const Identifiers& ids = identifiers();
  if (ids.empty()) {
    return {};
  }

  ManagerDetails managerDetails;

  for (const Identifier& identifier : ids) {
    const managerApi::ManagerInterfacePtr managerInterface =
        managerImplementationFactory_->instantiate(identifier);

    managerDetails.insert({identifier,
                           {managerInterface->identifier(), managerInterface->displayName(),
                            managerInterface->info()}});
  }
  return managerDetails;
}

ManagerPtr ManagerFactory::createManager(const Identifier& identifier) const {
  return createManagerForInterface(identifier, hostInterface_, managerImplementationFactory_,
                                   logger_);
}

ManagerPtr ManagerFactory::createManagerForInterface(
    const Identifier& identifier, const HostInterfacePtr& hostInterface,
    const ManagerImplementationFactoryInterfacePtr& managerImplementationFactory,
    const log::LoggerInterfacePtr& logger) {
  return Manager::make(
      managerImplementationFactory->instantiate(identifier),
      managerApi::HostSession::make(managerApi::Host::make(hostInterface), logger));
}

ManagerPtr ManagerFactory::defaultManagerForInterface(
    const HostInterfacePtr& hostInterface,
    const ManagerImplementationFactoryInterfacePtr& managerImplementationFactory,
    const log::LoggerInterfacePtr& logger) {
  const char* configPath = std::getenv(kDefaultManagerConfigEnvVarName.c_str());

  if (!configPath) {
    const Str msg =
        kDefaultManagerConfigEnvVarName + " not set, unable to instantiate default manager.";
    // We leave this as a debug message, as it is expected may hosts
    // will call this by default, and handle a null return manager, vs
    // it being a warning/error.
    logger->log(log::LoggerInterface::Severity::kDebug, msg);
    return nullptr;
  }
  {
    Str msg = "Retrieved default manager config file path from '";
    msg += kDefaultManagerConfigEnvVarName;
    msg += "'";
    logger->log(log::LoggerInterface::Severity::kDebug, msg);
  }

  return defaultManagerForInterface(configPath, hostInterface, managerImplementationFactory,
                                    logger);
}

namespace {

/**
 * Extract the plugin_paths option from TOML config as a PATH-like
 * string suitable for use in OPENASSETIO_PLUGIN_PATH.
 *
 * Returns empty optional if OPENASSETIO_PLUGIN_PATH is already set.
 *
 * @tparam SubsitutionFn Callable type to substitute placeholder variables in paths.
 * @param logger Logger to print warning if env var already set.
 * @param config Parsed TOML config from config file.
 * @param substitutionFn Callable to substitute any placeholder variables in paths.
 * @return PATH-like string, or empty optional.
 */
template <typename SubsitutionFn>
std::optional<std::string> pluginPathEnvVarFromConfig(const log::LoggerInterfacePtr& logger,
                                                      const toml::parse_result& config,
                                                      SubsitutionFn&& substitutionFn) {
  // Attempt to get array of paths from config file.
  const auto pluginPathsNode = config["manager"]["plugin_paths"];

  // If no plugin_paths then nothing to do.
  if (!pluginPathsNode) {
    return std::nullopt;
  }

  // If env var is already set, then it takes precedence over the
  // config file, so nothing to do.
  if (std::getenv(kPluginEnvVar.data())) {
    logger->warning(fmt::format("{} environment variable overrides plugin paths in config file",
                                kPluginEnvVar));
    return std::nullopt;
  }

  const auto* pluginPathsArray = pluginPathsNode.as_array();
  // Validate that plugin_paths is a toml array of string.
  if (!pluginPathsArray || !pluginPathsArray->is_homogeneous(toml::node_type::string)) {
    throw errors::ConfigurationException(
        "Invalid 'plugin_paths' value, must be an array of strings.");
  }

  // Substitute values into any placeholder strings in the search
  // paths.
  std::vector<std::string> pluginPaths;
  transform(std::cbegin(*pluginPathsArray), std::cend(*pluginPathsArray),
            back_inserter(pluginPaths), [&](const auto& pathNode) {
              return substitutionFn(pathNode.template value<std::string>().value());
            });

  // Concatenate search paths with platform-specific path separator.
  std::string pluginPathsAsEnvVar =
      fmt::format("{}", fmt::join(pluginPaths, std::string{kPathSep}));

  return pluginPathsAsEnvVar;
}

/**
 * RAII context manager to conditionally set, then unset, the
 * OPENASSETIO_PLUGIN_PATH env var, if a path is provided.
 */
struct PluginPathEnvVarCtx {
  std::optional<std::string> maybePluginPathEnvVar_;

  explicit PluginPathEnvVarCtx(std::optional<std::string> maybePluginPathEnvVar)
      : maybePluginPathEnvVar_{std::move(maybePluginPathEnvVar)} {
    if (maybePluginPathEnvVar_) {
#if defined(_WIN32)
      SetEnvironmentVariableA(kPluginEnvVar.data(), maybePluginPathEnvVar_->data());
#else
      setenv(kPluginEnvVar.data(), maybePluginPathEnvVar_->data(), 1);
#endif
    }
  }
  ~PluginPathEnvVarCtx() {
    if (maybePluginPathEnvVar_) {
#if defined(_WIN32)
      SetEnvironmentVariableA(kPluginEnvVar.data(), NULL);
#else
      unsetenv(kPluginEnvVar.data());
#endif
    }
  }
};
}  // namespace

ManagerPtr ManagerFactory::defaultManagerForInterface(
    const std::string_view configPath, const HostInterfacePtr& hostInterface,
    const ManagerImplementationFactoryInterfacePtr& managerImplementationFactory,
    const log::LoggerInterfacePtr& logger) {
  {
    Str msg = "Loading default manager config at '";
    msg += configPath;
    msg += "'";
    logger->log(log::LoggerInterface::Severity::kDebug, msg);
  }

  if (!std::filesystem::exists(configPath)) {
    Str msg = "Could not load default manager config from '";
    msg += configPath;
    msg += "', file does not exist.";
    throw errors::InputValidationException(msg);
  }

  if (std::filesystem::is_directory(configPath)) {
    Str msg = "Could not load default manager config from '";
    msg += configPath;
    msg += "', must be a TOML file not a directory.";
    throw errors::InputValidationException(msg);
  }

  toml::parse_result config;
  try {
    config = toml::parse_file(configPath);
  } catch (const std::exception& exc) {
    std::string msg = "Error parsing config file. ";
    msg += exc.what();
    throw errors::ConfigurationException{msg};
  }
  const std::string_view identifier = config["manager"]["identifier"].value_or("");

  // Function to substitute ${config_dir} with the absolute,
  // canonicalised directory of the TOML config file.
  const auto substituteConfigDir =
      [configDir =
           std::filesystem::canonical(configPath).parent_path().string()](std::string str) {
        // Adapted from https://en.cppreference.com/w/cpp/string/basic_string/replace
        for (std::string::size_type pos{};
             (pos = str.find(kConfigDirVar, pos)) != std::string::npos;
             pos += configDir.length()) {
          str.replace(pos, kConfigDirVar.length(), configDir);
        }
        return str;
      };

  InfoDictionary settings;
  if (toml::table* settingsTable = config["manager"]["settings"].as_table()) {
    // It'd be nice to use settingsTable::for_each, a lambda and
    // w/constexpr to filter supported types, filter, but it ends up
    // being somewhat verbose due to the number of types supported by
    // the variant.
    for (const auto& [key, val] : *settingsTable) {
      if (val.is_integer()) {
        settings.insert({Str{key}, val.as_integer()->get()});
      } else if (val.is_floating_point()) {
        settings.insert({Str{key}, val.as_floating_point()->get()});
      } else if (val.is_string()) {
        settings.insert({Str{key}, substituteConfigDir(val.as_string()->get())});
      } else if (val.is_boolean()) {
        settings.insert({Str{key}, val.as_boolean()->get()});
      } else {
        Str msg = "Unsupported value type for '";
        msg += key.str();
        msg += "'.";
        throw errors::ConfigurationException(msg);
      }
    }
  }

  // If OPENASSETIO_PLUGIN_PATH is not set and the config file has a
  // value for plugin_paths, (temporarily) set the env var to match
  // plugin_paths.
  [[maybe_unused]] const PluginPathEnvVarCtx pluginPathEnvVarCtx{
      pluginPathEnvVarFromConfig(logger, config, substituteConfigDir)};

  const managerApi::HostSessionPtr hostSession =
      managerApi::HostSession::make(managerApi::Host::make(hostInterface), logger);

  ManagerPtr manager = Manager::make(
      managerImplementationFactory->instantiate(Identifier(identifier)), hostSession);

  manager->initialize(settings);
  return manager;
}
}  // namespace hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
