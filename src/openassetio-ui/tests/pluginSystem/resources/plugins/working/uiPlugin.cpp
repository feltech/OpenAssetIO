// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <memory>

#include <export.h>

#include <openassetio/ui/pluginSystem/CppPluginSystemUIDelegatePlugin.hpp>

#include "StubUIDelegateInterface.hpp"

struct Plugin : openassetio::ui::pluginSystem::CppPluginSystemUIDelegatePlugin {
  [[nodiscard]] openassetio::Identifier identifier() const override {
    return "org.openassetio.test.pluginSystem."
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
