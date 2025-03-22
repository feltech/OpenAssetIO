// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 The Foundry Visionmongers Ltd
#pragma once

#include <any>
#include <functional>
#include <optional>
#include <utility>

#include <openassetio/export.h>
#include <openassetio/ui/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>

OPENASSETIO_FWD_DECLARE(ui::hostApi, UIDelegateState)

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::hostApi {

OPENASSETIO_DECLARE_PTR(UIDelegateRequestInterface)

/**
 * Abstract interface encapsulating a @ref UIDelegate "UI delegate"
 * request that may change over time.
 *
 * The @ref host provides instances of this class to a UI delegate,
 * which uses it to retrieve information that is relevant to the request
 * for UI delegation, as well as notify the host of relevant updates to
 * the UI delegate's internal @ref UIDelegateState "state".
 */
class OPENASSETIO_UI_EXPORT UIDelegateRequestInterface {
 public:
  OPENASSETIO_ALIAS_PTR(UIDelegateRequestInterface)
  /// Callback type for the UI delegate to notify the host of state
  /// changes.
  using StateChangedCallback = std::function<void(UIDelegateStatePtr)>;

  /// Defaulted destructor.
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
  [[nodiscard]] virtual std::any nativeData();

  /**
   * List of entity references associated with the request.
   *
   * For example, this could be the "current selection", for which the
   * UI delegate is expected to provide actions or additional
   * information.
   */
  [[nodiscard]] virtual EntityReferences entityReferences();

  /**
   * List of traits and their properties associated with the request.
   *
   * For example, this could be data that's intended to be published,
   * allowing the UI delegate a chance to augment or finesse the
   * published data.
   */
  [[nodiscard]] virtual trait::TraitsDatas entityTraitsDatas();

  /**
   * Callback to be called by the UI delegate to notify the host of
   * state changes.
   *
   * For example, when the user selects an entity in a browser.
   *
   * @note The state provided to the callback is a reference type, i.e.
   * it may be the same underlying instance as a previous state, so
   * checking equality of successive states is insufficient when
   * computing changes. Instead, extract the relevant elements of a
   * state early (e.g. entityReferences()) to use for subsequent
   * comparisons.
   */
  [[nodiscard]] virtual std::optional<StateChangedCallback> stateChangedCallback();
};
}  // namespace ui::hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
