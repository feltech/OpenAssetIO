// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/typedefs.hpp>

OPENASSETIO_FWD_DECLARE(hostApi, HostInterface)
OPENASSETIO_FWD_DECLARE(ui::hostApi, UIDelegate)
OPENASSETIO_FWD_DECLARE(ui::hostApi, UIDelegateImplementationFactoryInterface)
OPENASSETIO_FWD_DECLARE(log, LoggerInterface)
OPENASSETIO_FWD_DECLARE(uiDelegateApi, UIDelegateInterface)

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::hostApi {

OPENASSETIO_DECLARE_PTR(UIDelegateFactory)
using HostInterfacePtr = openassetio::hostApi::HostInterfacePtr;

/**
 * The UIDelegateFactory is the primary mechanism for querying for
 * available UI delegates and constructing a
 * @fqref{ui.hostApi.UIDelegate} "UIDelegate".
 *
 * The underlying uiDelegate implementation is constructed using the
 * supplied \fqref{ui.hostApi.UIDelegateImplementationFactoryInterface}
 * "UIDelegateImplementationFactoryInterface" factory implementation.
 *
 * Hosts should never attempt to directly construct a `UIDelegate` class
 * or interact with the implementation factory directly.
 */
class OPENASSETIO_UI_EXPORT UIDelegateFactory final {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateFactory)

  /**
   * Simple struct containing the default configuration details of a
   * potential UI delegate implementation.
   */
  struct UIDelegateDetail {
    /**
     * Identifier of the uiDelegate.
     *
     * @see @fqref{ui.hostApi.UIDelegate.identifier}
     * "UIDelegate.identifier"
     */
    Identifier identifier;
    /**
     * Human readable display name of the uiDelegate, suitable for
     * presenting in a UI.
     *
     * @see @fqref{ui.hostApi.UIDelegate.displayName}
     * "UIDelegate.displayName"
     */
    Str displayName;
    /**
     * Arbitrary key-value information supplied by the uiDelegate.
     *
     * @see @fqref{ui.hostApi.UIDelegate.info} "UIDelegate.info"
     */
    InfoDictionary info;
    /**
     * Compare all fields in this instance and another for by-value
     * equality.
     *
     * @param other Other instance to compare against.
     *
     * @return `true` if all fields compare equal, `false` otherwise.
     */
    bool operator==(const UIDelegateDetail& other) const {
      return identifier == other.identifier && displayName == other.displayName &&
             info == other.info;
    }
  };
  /// Mapping of UI delegate identifier to its configuration details.
  using UIDelegateDetails = std::unordered_map<Identifier, UIDelegateDetail>;

  /**
   * The name of the env var used to define the default UI delegate
   * config TOML file.
   *
    @see @ref defaultUIDelegateForInterface.
   */
  static const Str kDefaultUIDelegateConfigEnvVarName;

  /**
   * Construct an instance of this class.
   *
   * @param hostInterface The @ref host "host's" implementation of the
   * `HostInterface` that uniquely identifies the host and provides
   * common hooks for the @ref uiDelegate to query asset-related
   * properties from the host.
   *
   * @param uiDelegateImplementationFactory The factory that will be
   * used to instantiate UI delegates. See, for example, @ref
   * pluginSystem.PythonPluginSystemUIDelegateImplementationFactory.PythonPluginSystemUIDelegateImplementationFactory
   * "PythonPluginSystemUIDelegateImplementationFactory".
   *
   * @param logger The logger instance that will be used for all
   * messaging from the factory and instantiated
   * @fqref{ui.hostApi.UIDelegate} "UIDelegate" instances.
   */
  [[nodiscard]] static UIDelegateFactoryPtr make(
      HostInterfacePtr hostInterface,
      UIDelegateImplementationFactoryInterfacePtr uiDelegateImplementationFactory,
      log::LoggerInterfacePtr logger);

  /**
   * All identifiers known to the factory.
   *
   * @note This may result in a significant amount of work being
   * performed by the supplied UI delegate interface factory.
   *
   * @see @fqref{ui.hostApi.UIDelegate.identifier} "UIDelegate.identifier".
   */
  [[nodiscard]] Identifiers identifiers() const;

  /**
   * Get the details for each available @ref uiDelegate as a map of
   * UI delegate identifier to UI delegate details.
   *
   * This provides the default settings that can be taken and mutated
   * before being used in the initialization of a
   * @fqref{ui.hostApi.UIDelegate} "UIDelegate".
   *
   * Additional uiDelegate metadata is also included that may be useful.
   * For example, this may be presented as part of a uiDelegate picker
   * UI widget.
   *
   * @see @ref UIDelegateDetail
   *
   * @return A @ref UIDelegateDetail instance for each available @ref
   * uiDelegate.
   */
  [[nodiscard]] UIDelegateDetails availableUIDelegates() const;

  /**
   * Create a @fqref{ui.hostApi.UIDelegate} "UIDelegate" instance for
   * the UI delegate associated with the given identifier.
   *
   * The instance returned should then be used for all interaction with
   * the UI delegate.
   *
   * @param identifier Unique UI delegate identifier.
   *
   * @return Newly instantiated UI delegate.
   */
  [[nodiscard]] UIDelegatePtr createUIDelegate(const Identifier& identifier) const;

  /**
   * Create a @fqref{ui.hostApi.UIDelegate} "UIDelegate" instance for
   * the @ref UI delegate associated with the given identifier.
   *
   * The instance returned should then be used for all interaction with
   * the UI delegate.
   *
   * @param identifier Unique UI delegate identifier.
   *
   * @param hostInterface The @ref host "host's" implementation of the
   * `HostInterface` that uniquely identifies the host and provides
   * common hooks for the UI delegate to query asset-related properties
   * from the host.
   *
   * @param uiDelegateImplementationFactory The factory that will be used to
   * instantiate the UI delegate. See, for example, @ref
   * pluginSystem.PythonPluginSystemUIDelegateImplementationFactory.PythonPluginSystemUIDelegateImplementationFactory
   * "PythonPluginSystemUIDelegateImplementationFactory".
   *
   * @param logger The logger instance that will be used for all
   * messaging from the factory and instantiated
   * @fqref{ui.hostApi.UIDelegate} "UIDelegate" instances.
   *
   * @return Newly instantiated UI delegate.
   */
  [[nodiscard]] static UIDelegatePtr createUIDelegateForInterface(
      const Identifier& identifier, const HostInterfacePtr& hostInterface,
      const UIDelegateImplementationFactoryInterfacePtr& uiDelegateImplementationFactory,
      const log::LoggerInterfacePtr& logger);

  /**
   * Creates the default @fqref{ui.hostApi.UIDelegate} "UIDelegate" as
   * defined by the TOML configuration file referenced by the @ref
   * default_config_var.
   *
   * @note This mechanism should be the default approach for a host to
   * initialize the API. Extended functionality to override this
   * configuration can optionally be provided, but the ability to use
   * the shared, default configuration is always required.
   *
   * @see @ref defaultUIDelegateForInterface(std::string_view, <!--
   * -->const HostInterfacePtr&,<!--
   * -->const UIDelegateImplementationFactoryInterfacePtr&,<!--
   * -->const log::LoggerInterfacePtr&) "Alternative direct signature"
   * for more details.
   *
   * @envvar **OPENASSETIO_DEFAULT_CONFIG** *str* The path to a
   * TOML file containing configuration information for the default
   * UI delegate.
   *
   * @returns A default-configured UI delegate if
   * @ref default_config_var is set, otherwise a nullptr if
   * the var was not set.
   *
   * @throws errors.InputValidationException if there are errors if the
   * config file does not exist at the path provided in the @ref
   * default_config_var env var.
   *
   * @throws errors.ConfigurationException if there are errors occur
   * whilst loading the TOML file referenced by the @ref
   * default_config_var env var.
   */
  [[nodiscard]] static UIDelegatePtr defaultUIDelegateForInterface(
      const HostInterfacePtr& hostInterface,
      const UIDelegateImplementationFactoryInterfacePtr& uiDelegateImplementationFactory,
      const log::LoggerInterfacePtr& logger);

  /**
   * Creates the default @fqref{ui.hostApi.UIDelegate} "UIDelegate" as
   * defined by the given TOML configuration file.
   *
   * This allows deployments to centralize OpenAssetIO UI delegate settings,
   * and for hosts to instantiate this UI delegate without the need for
   * their own settings and persistence mechanism.
   *
   * The referenced TOML file should have the following structure.
   *
   * @code{.toml}
   * [uiDelegate]
   * identifier = "some.identifier"
   *
   * [uiDelegate.settings]  # Optional
   * some_setting = "value"
   * @endcode
   *
   * Any occurrences of `${config_dir}` within TOML string values will
   * be substituted with the absolute path to the directory containing
   * the TOML file, before being passed on to the UI delegate settings.
   *
   * @param configPath Path to the TOML config file, compatible with
   * <a href="https://en.cppreference.com/w/cpp/io/basic_ifstream/open">
   * `std::ifstream::open`</a>. Relative paths resolve to a
   * platform/environment-dependent location.
   *
   * @param hostInterface The @ref host "host's" implementation of the
   * `HostInterface` that uniquely identifies the host and provides
   * common hooks for the UI delegate to query asset-related properties
   * from the host.
   *
   * @param uiDelegateImplementationFactory The factory that will be
   * used to instantiate uiDelegates.
   *
   * @param logger The logger instance that will be used for all
   * messaging from the instantiated @fqref{ui.hostApi.UIDelegate}
   * "UIDelegate" instances.
   *
   * @return A default-configured UI delegate.
   *
   * @throws errors.InputValidationException if there are errors if the
   * config file does not exist at the path provided in @p configPath.
   *
   * @throws errors.ConfigurationException if there are errors occur
   * whilst loading the TOML file.
   */
  [[nodiscard]] static UIDelegatePtr defaultUIDelegateForInterface(
      std::string_view configPath, const HostInterfacePtr& hostInterface,
      const UIDelegateImplementationFactoryInterfacePtr& uiDelegateImplementationFactory,
      const log::LoggerInterfacePtr& logger);

 private:
  UIDelegateFactory(HostInterfacePtr hostInterface,
                    UIDelegateImplementationFactoryInterfacePtr uiDelegateImplementationFactory,
                    log::LoggerInterfacePtr logger);

  HostInterfacePtr hostInterface_;
  UIDelegateImplementationFactoryInterfacePtr uiDelegateImplementationFactory_;
  log::LoggerInterfacePtr logger_;
};

}  // namespace ui::hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
