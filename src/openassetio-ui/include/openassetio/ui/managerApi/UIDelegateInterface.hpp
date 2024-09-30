// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2024 The Foundry Visionmongers Ltd

#pragma once

#include <functional>
#include <optional>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/Context.hpp>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/access.hpp>

OPENASSETIO_FWD_DECLARE(managerApi, HostSession)
OPENASSETIO_FWD_DECLARE(Context)
OPENASSETIO_FWD_DECLARE(ui, UIDelegateRequestInterface)
OPENASSETIO_FWD_DECLARE(ui, UIDelegateStateInterface)

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::managerApi {

OPENASSETIO_DECLARE_PTR(UIDelegateInterface)

/**
 */
class OPENASSETIO_UI_EXPORT UIDelegateInterface {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateInterface)

  using HostSessionPtr = openassetio::managerApi::HostSessionPtr;

  /**
   * Constructor.
   *
   * No work is done here - UIDelegateInterface instances should be
   * cheap to construct. Any heavy-lifting should be done in @ref
   * initialize, where relevant settings are also available.
   */
  UIDelegateInterface();

  /**
   * Polymorphic destructor.
   */
  virtual ~UIDelegateInterface() = default;

  /**
   * @name Asset Management System Identification
   *
   * These functions provide hosts with general identity information
   * about the @ref asset_management_system "asset management system's"
   * UI delegate itself. These may all be called before @ref initialize
   * has been called.
   *
   * @{
   */

  /**
   * Returns an identifier to uniquely identify a specific asset
   * manager and its UI delegate.
   *
   * This may be used by a host to persist the users preferred manager
   * via a preferences mechanism, or when spawning child processes,
   * etc...
   *
   * It should match the name used to register the plug-in with the
   * plug-in host.  The identifier should use only alpha-numeric
   * characters and '.', '_' or '-'.  Generally speaking, we recommend
   * using the 'reverse-DNS' convention, for example:
   *
   *     "org.openassetio.test.manager"
   *
   * @return Unique identifier of the manager.
   *
   * @see https://en.wikipedia.org/wiki/Reverse_domain_name_notation
   */
  [[nodiscard]] virtual Identifier identifier() const = 0;

  /**
   * Returns a human readable name to be used to reference this
   * specific UI delegate in UIs or other user-facing messaging.
   *
   * One instance of its use may be in a host's preferences UI or
   * logging. For example:
   *
   *     "OpenAssetIO Test UI Delegate"
   *
   * @return UIDelegate's display name.
   */
  [[nodiscard]] virtual Str displayName() const = 0;

  /**
   * Returns other information that may be useful about this @ref
   * asset_management_system "asset management system's" UI delegate.
   * This can contain arbitrary key/value pairs. For example:
   *
   *     { 'version' : '1.1v3', 'server' : 'assets.openassetio.org' }
   *
   * There are certain optional keys that may be used by a host or
   * the API:
   *
   *   @li @ref constants.kInfoKey_SmallIcon (upto 32x32)
   *   @li @ref constants.kInfoKey_Icon (any size)
   *
   * @return Map of info string key to primitive value.
   */
  [[nodiscard]] virtual InfoDictionary info();

  /**
   * @}
   */

  /**
   * @name Initialization
   *
   * @{
   */

  /**
   * @param hostSession The API session.
   *
   * @return Any settings relevant to the function of the manager with
   * their current values (or their defaults if @ref initialize has
   * not yet been called).
   *
   * The default implementation returns an empty dictionary.
   */
  [[nodiscard]] virtual InfoDictionary settings(const HostSessionPtr& hostSession);

  /**
   * Prepares for interaction with a host.
   *
   * This method is passed a settings dictionary, that can be used to
   * configure required local state to service requests. For example,
   * determining the authoritive back-end service managing asset data.
   * This is also a good opportunity to initialize any connections or
   * fetch pre-requisite data. It is fine for this call to block for a
   * period of time.
   *
   * If an exception is raised by this call, it signifies to the host
   * that a fatal error occurred, and this @ref asset_management_system
   * "asset management system's" UI delegate is not available with the
   * current settings.
   *
   * If no exception is raised, it can be assumed that the @ref
   * asset_management_system "asset management system's" UI delegate is
   * ready. It is the implementations responsibility to deal with
   * transient connection errors (if applicable) once initialized.
   *
   * If called on an already initialized instance, re-initialize with
   * any updated settings that are provided. If an error was raised
   * previously, then initialization should be re-attempted.
   *
   * @note This will always be called prior to any UI-related
   * calls. An exception should be raised if this is not the case.
   * However, the following functions may be called prior to
   * initialization:
   *
   *  @li @ref identifier()
   *  @li @ref displayName()
   *  @li @ref info()
   *  @li @ref settings()
   */
  virtual void initialize(InfoDictionary uiDelegateSettings, const HostSessionPtr& hostSession);

  /**
   * Clear all data related to active UI elements, especially callbacks.
   *
   * Called automatically on destruction of the @ref
   * managerApi.UIDelegate middleware in use by the host.
   */
  virtual void close(const HostSessionPtr& hostSession);

  /**
   * Clears any internal caches.
   *
   * Only applicable if the implementation makes use of any caching,
   * otherwise it is a no-op. In caching interfaces, this will cause any
   * retained data to be discarded to ensure future queries are fresh.
   *
   * @param hostSession The API session.
   */
  virtual void flushCaches(const HostSessionPtr& hostSession);

  /**
   * Introspect the UI delegate for its policy with respect to the
   * given UI traits, access mode, context and host.
   *
   * This mirrors the
   * @fqref{manaerApi.ManagerInterface.managementPolicy}
   * "managementPolicy" method for manager implementations.
   */
  [[nodiscard]] virtual trait::TraitsDataPtr uiPolicy(const trait::TraitSet& uiTraits,
                                                      access::UIAccess uiAccess,
                                                      const ContextConstPtr& context,
                                                      const HostSessionPtr& hostSession);

  /**
   * Populate a UI element on behalf of the host.
   *
   * The nature of the UI to populate, how it should be populated, and
   * what communication channels should be set up with the host, is
   * determined by the UI traits, access mode, request object, context
   * and host parameters.
   */
  virtual UIDelegateStateInterfacePtr populateUI(const trait::TraitsDataConstPtr& uiTraitsData,
                                                 access::UIAccess uiAccess,
                                                 const UIDelegateRequestInterfacePtr& requestState,
                                                 const ContextConstPtr& context,
                                                 const HostSessionPtr& hostSession);
  /**
   * @}
   */
};
}  // namespace ui::managerApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
