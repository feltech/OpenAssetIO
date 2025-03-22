// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 The Foundry Visionmongers Ltd
#pragma once

#include <any>
#include <functional>
#include <utility>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>

OPENASSETIO_FWD_DECLARE(TraitsData)

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui {

class UIDelegateStateInterface;

OPENASSETIO_DECLARE_PTR(UIDelegateRequestInterface)
OPENASSETIO_DECLARE_PTR(UIDelegateStateInterface)

/**
 * Abstract interface encapsulating a @ref hostApi.UIDelegate "UI
 * delegate" request that may change over time.
 *
 * The @ref host provides instances of this class to a UI delegate,
 * which uses it to retrieve information that is relevant to the request
 * for UI delegation, as well as notify the host of relevant updates to
 * the UI delegate's internal @ref UIDelegateStateInterface "state".
 */
class OPENASSETIO_UI_EXPORT UIDelegateRequestInterface {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateRequestInterface)
  /// Callback type for the UI delegate to notify the host of state
  /// changes.
  using StateChangedCallback = std::function<void(const UIDelegateStateInterfacePtr&)>;

  virtual ~UIDelegateRequestInterface() = default;

  /**
   * Arbitrary data object included with the request.
   *
   * It is up to the host to document what will be placed in here, if
   * anything.
   *
   * For example, it could be a container widget to be populated
   * by the UI delegate.
   *
   * Note that for Python UI delegates, this must contain a CPython
   * `PyObject*`.
   */
  [[nodiscard]] virtual std::any nativeData() = 0;

  /**
   * List of entity references associated with the request.
   *
   * For example, this could be the "current selection", for which the
   * UI delegate is expected to provide actions or additional
   * information.
   */
  [[nodiscard]] virtual EntityReferences entityReferences() = 0;

  /**
   * List of traits and their properties associated with the request.
   *
   * For example, this could be data that's intended to be published,
   * allowing the UI delegate a chance to augment or finesse the
   * published data.
   */
  [[nodiscard]] virtual trait::TraitsDatas entityTraitsDatas() = 0;

  /**
   * Callback to be called by the UI delegate to notify the host of
   * state changes.
   *
   * For example, when the user selects an entity in a browser.
   */
  [[nodiscard]] virtual StateChangedCallback stateChangedCallback() = 0;
};

/**
 * Abstract interface encapsulating the @ref
 * managerApi.UIDelegateInterface "UI delegate" state that may change
 * over time.
 *
 * The UI delegate provides instances of this class to a @ref host to
 * notify the host of relevant changes to its internal state, as well as
 * allow the host to update the initial @ref UIDelegateRequestInterface
 * "request" with new information.
 */
class OPENASSETIO_UI_EXPORT UIDelegateStateInterface {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateStateInterface)
  /// Callback to type for the host to notify the UI delegate of changes
  /// to the initial request.
  using UpdateRequestCallback = std::function<void(const UIDelegateRequestInterfacePtr&)>;

  virtual ~UIDelegateStateInterface() = default;

  /**
   * Arbitrary data object included with the state.
   *
   * It is up to the host to document what should be placed in here, if
   * anything.
   *
   * For example, it could be the top-level widget created by the UI
   * delegate, ready to be inserted into the UI hierarchy by the host.
   *
   * Note that for Python hosts, this must contain a CPython
   * `PyObject*`.
   */
  [[nodiscard]] virtual std::any nativeData() = 0;

  /**
   * List of entity references considered relevant for the host by the
   * UI delegate.
   *
   * For example, this could be the entities chosen by the user in a
   * browser.
   */
  [[nodiscard]] virtual EntityReferences entityReferences() = 0;

  /**
   * List of traits and their properties considered relevant for the
   * host by the UI delegate.
   *
   * For example, this could include additional data to be published,
   * which the host may or may not wish to further process.
   */
  [[nodiscard]] virtual trait::TraitsDatas entityTraitsDatas() = 0;

  /**
   * Callback to be called by the host to notify the UI delegate that
   * the request has changed.
   *
   * For example, if the current selection has changed.
   */
  [[nodiscard]] virtual UpdateRequestCallback updateRequestCallback() = 0;
};

}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
