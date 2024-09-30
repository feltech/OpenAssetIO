// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/pluginSystem/CppPluginSystemPlugin.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::pluginSystem {

OPENASSETIO_DECLARE_PTR(CppPluginSystemUIDelegatePlugin)
using CppPluginSystemPlugin = openassetio::pluginSystem::CppPluginSystemPlugin;
/**
 * Base class to be subclassed by plugins binding a @ref host to an @ref
 * asset_management_system "asset management system's" UI delegate.
 *
 * This is used by the dynamic plugin discovery mechanism to instantiate
 * the @ref ui.managerApi.UIDelegateInterface "UIDelegateInterface"
 * implementation for the asset management system.
 *
 * Plugin authors must subclass this class and expose instances of it
 * via a @ref PluginFactory function pointer, which is in turn exposed
 * in the plugin binary by a top level C linkage `openassetioUIPlugin`
 * function.
 *
 * @see CppPluginSystemUIDelegateImplementationFactory
 */
class OPENASSETIO_UI_EXPORT CppPluginSystemUIDelegatePlugin : public CppPluginSystemPlugin {
 public:
  OPENASSETIO_ALIAS_PTR(CppPluginSystemUIDelegatePlugin)

  /// No-op destructor.
  ~CppPluginSystemUIDelegatePlugin() override;

  /**
   * Constructs an instance of the @ref managerApi.UIDelegateInterface
   * "UIDelegateInterface".
   *
   * This is an instance of some class derived from UIDelegateInterface
   * to be bound to the Host-facing @ref hostApi.UIDelegate
   * "UIDelegate".
   *
   * Generally this is only directly called by the @ref
   * pluginSystem.CppPluginSystemUIDelegateImplementationFactory
   * "CppPluginSystemUIDelegateImplementationFactory".
   *
   * @return UIDelegateInterface instance.
   */
  [[nodiscard]] virtual ui::managerApi::UIDelegateInterfacePtr interface() = 0;
};
}  // namespace ui::pluginSystem
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
