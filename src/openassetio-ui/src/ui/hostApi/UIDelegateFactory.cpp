// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <string>
#include <string_view>
#include <utility>

#include <toml++/toml.h>

#include <openassetio/export.h>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/log/LoggerInterface.hpp>
#include <openassetio/managerApi/Host.hpp>
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/hostApi/UIDelegateFactory.hpp>
#include <openassetio/ui/hostApi/UIDelegateImplementationFactoryInterface.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

namespace {
constexpr std::string_view kConfigDirVar = "${config_dir}";
}  // namespace

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::hostApi {
using Host = openassetio::managerApi::Host;
using HostSession = openassetio::managerApi::HostSession;
using HostSessionPtr = openassetio::managerApi::HostSessionPtr;

const Str UIDelegateFactory::kDefaultUIDelegateConfigEnvVarName = "OPENASSETIO_DEFAULT_CONFIG";

UIDelegateFactoryPtr UIDelegateFactory::make(
    HostInterfacePtr hostInterface,
    UIDelegateImplementationFactoryInterfacePtr uiDelegateImplementationFactory,
    log::LoggerInterfacePtr logger) {
  return UIDelegateFactoryPtr{new UIDelegateFactory{
      std::move(hostInterface), std::move(uiDelegateImplementationFactory), std::move(logger)}};
}

UIDelegateFactory::UIDelegateFactory(
    HostInterfacePtr hostInterface,
    UIDelegateImplementationFactoryInterfacePtr uiDelegateImplementationFactory,
    log::LoggerInterfacePtr logger)
    : hostInterface_{std::move(hostInterface)},
      uiDelegateImplementationFactory_{std::move(uiDelegateImplementationFactory)},
      logger_{std::move(logger)} {}

Identifiers UIDelegateFactory::identifiers() const {
  return uiDelegateImplementationFactory_->identifiers();
}

UIDelegateFactory::UIDelegateDetails UIDelegateFactory::availableUIDelegates() const {
  const Identifiers& ids = identifiers();
  if (ids.empty()) {
    return {};
  }

  UIDelegateDetails uiDelegateDetails;

  for (const Identifier& identifier : ids) {
    const managerApi::UIDelegateInterfacePtr uiDelegateInterface =
        uiDelegateImplementationFactory_->instantiate(identifier);

    uiDelegateDetails.insert({identifier,
                              {uiDelegateInterface->identifier(),
                               uiDelegateInterface->displayName(), uiDelegateInterface->info()}});
  }
  return uiDelegateDetails;
}

UIDelegatePtr UIDelegateFactory::createUIDelegate(const Identifier& identifier) const {
  return createUIDelegateForInterface(identifier, hostInterface_, uiDelegateImplementationFactory_,
                                      logger_);
}

UIDelegatePtr UIDelegateFactory::createUIDelegateForInterface(
    const Identifier& identifier, const HostInterfacePtr& hostInterface,
    const UIDelegateImplementationFactoryInterfacePtr& uiDelegateImplementationFactory,
    const log::LoggerInterfacePtr& logger) {
  return UIDelegate::make(uiDelegateImplementationFactory->instantiate(identifier),
                          HostSession::make(Host::make(hostInterface), logger));
}

UIDelegatePtr UIDelegateFactory::defaultUIDelegateForInterface(
    const HostInterfacePtr& hostInterface,
    const UIDelegateImplementationFactoryInterfacePtr& uiDelegateImplementationFactory,
    const log::LoggerInterfacePtr& logger) {
  const char* configPath = std::getenv(kDefaultUIDelegateConfigEnvVarName.c_str());

  if (!configPath) {
    const Str msg =
        kDefaultUIDelegateConfigEnvVarName + " not set, unable to instantiate default uiDelegate.";
    // We leave this as a debug message, as it is expected may hosts
    // will call this by default, and handle a null return UI delegate,
    // vs it being a warning/error.
    logger->log(log::LoggerInterface::Severity::kDebug, msg);
    return nullptr;
  }
  {
    Str msg = "Retrieved default uiDelegate config file path from '";
    msg += kDefaultUIDelegateConfigEnvVarName;
    msg += "'";
    logger->log(log::LoggerInterface::Severity::kDebug, msg);
  }

  return defaultUIDelegateForInterface(configPath, hostInterface, uiDelegateImplementationFactory,
                                       logger);
}

UIDelegatePtr UIDelegateFactory::defaultUIDelegateForInterface(
    const std::string_view configPath, const HostInterfacePtr& hostInterface,
    const UIDelegateImplementationFactoryInterfacePtr& uiDelegateImplementationFactory,
    const log::LoggerInterfacePtr& logger) {
  {
    Str msg = "Loading default UI delegate config at '";
    msg += configPath;
    msg += "'";
    logger->log(log::LoggerInterface::Severity::kDebug, msg);
  }

  if (!std::filesystem::exists(configPath)) {
    Str msg = "Could not load default UI delegate config from '";
    msg += configPath;
    msg += "', file does not exist.";
    throw errors::InputValidationException(msg);
  }

  if (std::filesystem::is_directory(configPath)) {
    Str msg = "Could not load default UI delegate config from '";
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
  if (toml::table* settingsTable = config["uiDelegate"]["settings"].as_table()) {
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

  const HostSessionPtr hostSession = HostSession::make(Host::make(hostInterface), logger);

  UIDelegatePtr uiDelegate = UIDelegate::make(
      uiDelegateImplementationFactory->instantiate(Identifier(identifier)), hostSession);

  uiDelegate->initialize(settings);
  return uiDelegate;
}
}  // namespace ui::hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
