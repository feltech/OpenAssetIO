// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <openassetio/errors/exceptions.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

struct StubUIDelegateInterface : openassetio::ui::managerApi::UIDelegateInterface {
  [[nodiscard]] openassetio::Identifier identifier() const override {
    return "org.openassetio.test.pluginSystem."
           "resources." OPENASSETIO_UI_PLUGINSYSTEM_TEST_PLUGIN_ID_SUFFIX;
  }

  [[nodiscard]] openassetio::Str displayName() const override {
    return OPENASSETIO_UI_PLUGINSYSTEM_TEST_PLUGIN_ID_SUFFIX;
  }

  // Deliberately throw an exception, for use in checking RTTI.
  openassetio::InfoDictionary info() override {
    throw openassetio::errors::NotImplementedException{"Stub doesn't support info"};
  }

  void close([[maybe_unused]] const HostSessionPtr& hostSession) override {}
};
