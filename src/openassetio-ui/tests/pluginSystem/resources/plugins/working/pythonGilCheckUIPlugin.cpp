// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <memory>
#include <stdexcept>

#include <export.h>

#include <Python.h>

#include <openassetio/pluginSystem/CppPluginSystemPlugin.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>
#include <openassetio/ui/pluginSystem/CppPluginSystemUIDelegatePlugin.hpp>

#include "StubUIDelegateInterface.hpp"

struct Plugin final : openassetio::ui::pluginSystem::CppPluginSystemUIDelegatePlugin {
  [[nodiscard]] openassetio::Str identifier() const override {
    if (PyGILState_Check()) {
      throw std::runtime_error{"GIL was not released when identifying C++ plugin"};
    }
    return "org.openassetio.test.pluginSystem."
           // NOLINTNEXTLINE(misc-include-cleaner) - definition provided on command line.
           "resources." OPENASSETIO_UI_PLUGINSYSTEM_TEST_PLUGIN_ID_SUFFIX;
  }
  openassetio::ui::managerApi::UIDelegateInterfacePtr interface() override {
    if (PyGILState_Check()) {
      throw std::runtime_error{
          "GIL was not released when instantiating UI delegate from C++ plugin"};
    }
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
