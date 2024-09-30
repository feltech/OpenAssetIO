// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once
#include <vector>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/hostApi/UIDelegateImplementationFactoryInterface.hpp>

OPENASSETIO_FWD_DECLARE(ui::managerApi, UIDelegateInterface)

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::pluginSystem {
OPENASSETIO_DECLARE_PTR(HybridPluginSystemUIDelegateImplementationFactory)

/**
 * The hybrid plugin system composes one or more child plugin systems,
 * and selects the first plugin that matches the desired identifier.
 *
 * A list of factories are provided in priority order. When a plugin
 * with a particular identifier is requested, all factories are queried
 * and returns the first that returns positively for the identifier.
 */
class OPENASSETIO_UI_EXPORT HybridPluginSystemUIDelegateImplementationFactory
    : public hostApi::UIDelegateImplementationFactoryInterface {
 public:
  using UIDelegateImplementationFactoryInterfaces =
      std::vector<hostApi::UIDelegateImplementationFactoryInterfacePtr>;

  OPENASSETIO_ALIAS_PTR(HybridPluginSystemUIDelegateImplementationFactory)

  /**
   * Construct a new instance.
   *
   * @param factories List of factories to compose.
   *
   * @param logger Logger for progress and warnings.
   *
   * @return New instance.
   */
  static HybridPluginSystemUIDelegateImplementationFactoryPtr make(
      UIDelegateImplementationFactoryInterfaces factories, log::LoggerInterfacePtr logger);

  /**
   * Get a list of all manager plugin identifiers known to all child
   * factories.
   *
   * @return List of known manager plugin identifiers.
   */
  Identifiers identifiers() override;

  /**
   * Create an instance of the @ref managerApi.UIDelegateInterface
   * "UIDelegateInterface" with the specified identifier.
   *
   * Note that, like any other plugin system, the returned
   * `UIDelegateInterface` cannot be used until @ref
   * managerApi.UIDelegateInterface.initialize "initialized".
   *
   * @param identifier Identifier of the `UIDelegateInterface` to
   * instantiate.
   *
   * @return Newly created interface.
   *
   * @throws InputValidationException if the requested identifier has
   * not been registered as a manager plugin.
   */
  managerApi::UIDelegateInterfacePtr instantiate(const Identifier& identifier) override;

 private:
  /// Private constructor. See @ref make.
  explicit HybridPluginSystemUIDelegateImplementationFactory(
      UIDelegateImplementationFactoryInterfaces factories, log::LoggerInterfacePtr logger);

  /// Child factories to compose.
  UIDelegateImplementationFactoryInterfaces factories_;
};
}  // namespace ui::pluginSystem
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
