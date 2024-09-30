// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <memory>

#include <export.h>

#include <openassetio/ui/pluginSystem/CppPluginSystemUIDelegatePlugin.hpp>

#include "StubUIDelegateInterface.hpp"
#include "openassetio/pluginSystem/CppPluginSystemPlugin.hpp"
#include "openassetio/typedefs.hpp"
#include "openassetio/ui/managerApi/UIDelegateInterface.hpp"

struct Plugin : openassetio::ui::pluginSystem::CppPluginSystemUIDelegatePlugin {
  [[nodiscard]] openassetio::Identifier identifier() const override {
    return "org.openassetio.test.pluginSystem."
           // NOLINTNEXTLINE(misc-include-cleaner) - definition provided on command line.
           "resources." OPENASSETIO_UI_PLUGINSYSTEM_TEST_PLUGIN_ID_SUFFIX;
  }
  openassetio::ui::managerApi::UIDelegateInterfacePtr interface() override {
    return std::make_shared<StubUIDelegateInterface>();
  }
};

extern "C" {

OPENASSETIO_UI_PLUGINSYSTEM_TEST_EXPORT
openassetio::pluginSystem::PluginFactory openassetioPlugin() noexcept {
  return []() noexcept -> openassetio::pluginSystem::CppPluginSystemPluginPtr {
    return std::make_shared<Plugin>();
  };
}
}
