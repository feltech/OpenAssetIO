// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <algorithm>
#include <cassert>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include <openassetio/export.h>
#include <openassetio/errors/exceptions.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/hostApi/UIDelegateImplementationFactoryInterface.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>
#include <openassetio/ui/pluginSystem/HybridPluginSystemUIDelegateImplementationFactory.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::pluginSystem {

HybridPluginSystemUIDelegateImplementationFactoryPtr
HybridPluginSystemUIDelegateImplementationFactory::make(
    UIDelegateImplementationFactoryInterfaces factories, log::LoggerInterfacePtr logger) {
  if (factories.empty()) {
    throw errors::InputValidationException{
        "HybridPluginSystem: At least one child manager implementation factory must be provided"};
  }

  return std::make_shared<HybridPluginSystemUIDelegateImplementationFactory>(
      HybridPluginSystemUIDelegateImplementationFactory{std::move(factories), std::move(logger)});
}

HybridPluginSystemUIDelegateImplementationFactory::
    HybridPluginSystemUIDelegateImplementationFactory(
        UIDelegateImplementationFactoryInterfaces factories, log::LoggerInterfacePtr logger)
    : UIDelegateImplementationFactoryInterface{std::move(logger)},
      factories_{std::move(factories)} {}

Identifiers HybridPluginSystemUIDelegateImplementationFactory::identifiers() {
  Identifiers identifiers;
  // Collect all identifiers from all factories.
  for (const auto& factory : factories_) {
    Identifiers factoryIdentifiers = factory->identifiers();
    identifiers.insert(end(identifiers), make_move_iterator(begin(factoryIdentifiers)),
                       make_move_iterator(end(factoryIdentifiers)));
  }
  // Sort and remove duplicates.
  sort(begin(identifiers), end(identifiers));
  identifiers.erase(unique(begin(identifiers), end(identifiers)), end(identifiers));
  return identifiers;
}

managerApi::UIDelegateInterfacePtr HybridPluginSystemUIDelegateImplementationFactory::instantiate(
    const Identifier& identifier) {
  std::vector<managerApi::UIDelegateInterfacePtr> managerInterfaces;

  for (const auto& factory : factories_) {
    const Identifiers& factoryIdentifiers = factory->identifiers();
    if (const auto iter = find(cbegin(factoryIdentifiers), cend(factoryIdentifiers), identifier);
        iter != cend(factoryIdentifiers)) {
      managerInterfaces.push_back(factory->instantiate(identifier));
    }
  }

  if (managerInterfaces.empty()) {
    throw errors::InputValidationException{fmt::format(
        "HybridPluginSystem: No plug-in registered with the identifier '{}'", identifier)};
  }

  return std::move(managerInterfaces.front());
}
}  // namespace ui::pluginSystem
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio