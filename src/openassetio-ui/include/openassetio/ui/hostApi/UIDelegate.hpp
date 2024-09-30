// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <any>
#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/Context.hpp>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/access.hpp>

OPENASSETIO_FWD_DECLARE(ui::managerApi, UIDelegateInterface)
OPENASSETIO_FWD_DECLARE(ui, UIDelegateStateInterface)
OPENASSETIO_FWD_DECLARE(ui, UIDelegateRequestInterface)
OPENASSETIO_FWD_DECLARE(managerApi, HostSession)
OPENASSETIO_FWD_DECLARE(Context)

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::hostApi {

OPENASSETIO_DECLARE_PTR(UIDelegate)

/**
 *
 */
class OPENASSETIO_UI_EXPORT UIDelegate final {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegate)

  UIDelegate(const UIDelegate& other) = delete;
  UIDelegate(UIDelegate&& other) noexcept = default;
  UIDelegate& operator=(const UIDelegate& other) = delete;
  UIDelegate& operator=(UIDelegate&& other) noexcept = default;
  ~UIDelegate();

  /**
   * Constructs a new UIDelegate wrapping the supplied UI delegate
   * interface and host session.
   */
  [[nodiscard]] static UIDelegatePtr make(managerApi::UIDelegateInterfacePtr uiDelegateInterface,
                                          openassetio::managerApi::HostSessionPtr hostSession);

  /**
   * @name Asset Management System UI Delegate Identification
   *
   * These functions provide general identity information about the @ref
   * asset_management_system "asset management system's" UI delegate
   * itself. These can all be called before @ref initialize has been
   * called.
   *
   * @{
   */

  /**
   * Returns an identifier to uniquely identify the UIDelegate.
   *
   * This identifier is used with the @fqref{hostApi.UIDelegateFactory}
   * "UIDelegateFactory" to select which UIDelegate to initialize, and
   * so can be used in preferences etc to persist the chosen UIDelegate.
   * The identifier will use only alpha-numeric characters and '.', '_'
   * or '-'. They generally follow the 'reverse-DNS' style, for example:
   *
   *     "org.openassetio.test.manager"
   */
  [[nodiscard]] Identifier identifier() const;

  /**
   * Returns a human readable name to be used to reference this
   * specific asset manager in user-facing displays.
   * For example:
   *
   *     "OpenAssetIO Test UIDelegate"
   */
  [[nodiscard]] Str displayName() const;

  /**
   * Returns other information that may be useful about this @ref
   * asset_management_system. This can contain arbitrary key/value
   * pairs.
   *
   * @return Map of info string key to primitive value.
   */
  [[nodiscard]] InfoDictionary info();

  /**
   * @}
   */

  /**
   * @name Initialization
   *
   * @{
   */

  /**
   * @return Any settings relevant to the function of the manager with
   * their current values (or their defaults if @ref initialize has
   * not yet been called).
   *
   * Some managers may not have any settings, so this function will
   * return an empty dictionary.
   */
  [[nodiscard]] InfoDictionary settings();

  /**
   * Prepares the UIDelegate for interaction with a host. In order to
   * provide light weight inspection of available UIDelegates, initial
   * construction must be cheap. However most system require some
   * kind of handshake or back-end setup in order to make
   * entity-related queries. As such, the @ref initialize method is
   * the instruction to the UIDelegate to prepare itself for full
   * interaction.
   *
   * If an exception is raised by this call, its is safe to assume
   * that a fatal error occurred, and this @ref
   * asset_management_system is not available, and should be retried
   * later.
   *
   * If no exception is raised, it can be assumed that the @ref "asset
   * management system's" UI delegate is ready. It is the
   * implementations responsibility to deal with transient connection
   * errors (if applicable) once initialized.
   *
   * The behavior of calling initialize() on an already initialized
   * UIDelegate is to re-initialize the UI delegate with any updated
   * settings that are provided. If an error was raised previously, then
   * initialization will be re-attempted.
   *
   * @note This must be called prior to any entity-related calls or
   * an Exception will be raised.
   *
   * @note This method may block for extended periods of time.
   */
  void initialize(InfoDictionary uiDelegateSettings);

  /**
   * @}
   */

  /**
   * Instruct the UI delegate to dispose of all active references to
   * delegated UI.
   *
   * Called automatically on destruction of this UIDelegate.
   */
  void close();

  /**
   * Clears any internal caches.
   *
   * Only applicable if the manager makes use of any caching, otherwise
   * it is a no-op.  In caching interfaces, this should cause any
   * retained data to be discarded to ensure future queries are fresh.
   */
  void flushCaches();

  [[nodiscard]] trait::TraitsDataPtr uiPolicy(const trait::TraitSet& uiTraits,
                                              access::UIAccess uiAccess,
                                              const ContextConstPtr& context);

  UIDelegateStateInterfacePtr populateUI(const trait::TraitsDataConstPtr& uiTraitsData,
                                         access::UIAccess uiAccess,
                                         const UIDelegateRequestInterfacePtr& requestState,
                                         const ContextConstPtr& context);

 private:
  explicit UIDelegate(managerApi::UIDelegateInterfacePtr uiDelegateInterface,
                      openassetio::managerApi::HostSessionPtr hostSession);

  managerApi::UIDelegateInterfacePtr uiDelegateInterface_;
  openassetio::managerApi::HostSessionPtr hostSession_;

  std::optional<Str> entityReferencePrefix_;
};
}  // namespace ui::hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
