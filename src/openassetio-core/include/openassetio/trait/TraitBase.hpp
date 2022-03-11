// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Base class for all specification traits.
 */
#pragma once
#include <algorithm>
#include <memory>
#include <utility>

#include "../specification/Specification.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace trait {

/**
 * Abstract CRTP base class for specification traits.
 *
 * @tparam Derived Concrete subclass.
 */
template <class Derived>
struct TraitBase {
  /// Ref-counted smart pointer to underlying Specification.
  using SpecificationPtr = std::shared_ptr<specification::Specification>;

  /**
   * Construct this trait, wrapping given specification.
   *
   * @param spec Underlying specification to wrap.
   */
  explicit TraitBase(SpecificationPtr spec) : spec_{std::move(spec)} {}

  /**
   * Get the ID of this trait.
   *
   * @return ID of this trait.
   */
  [[nodiscard]] static const TraitId& traitId() { return Derived::kId; }

  /**
   * Check whether the specification this trait has been applied to
   * actually supports this trait.
   *
   * @return `true` if the underlying specification supports this trait,
   * `false` otherwise.
   **/
  [[nodiscard]] bool isValid() const { return spec_->hasTrait(traitId()); }

 protected:
  /**
   * Get the underlying Specification that this trait is wrapping.
   *
   * @return Wrapped Specification.
   */
  SpecificationPtr& spec() { return spec_; }

  /**
   * Get the underlying Specification that this trait is wrapping.
   *
   * @return Wrapped Specification.
   */
  [[nodiscard]] const SpecificationPtr& spec() const { return spec_; }

  /**
   * Convenience typed accessor to properties in the underlying
   * Specification.
   *
   * @tparam T Type to extract from variant property.
   * @param[out] out Storage for value, if property is set.
   * @param traitId ID of trait to query.
   * @param propertyKey Key of property to query.
   * @return `true` if property is set, `false` otherwise.
   * @exception `std::bad_variant_access` if `T` does not match the type
   * stored in the property.
   */
  template <class T>
  [[nodiscard]] bool getTraitProperty(T* out, const TraitId& traitId,
                                      const property::Key& propertyKey) const {
    if (property::Value value; spec()->getTraitProperty(&value, traitId, propertyKey)) {
      // TODO(DF): Make exception message more friendly.
      *out = std::get<T>(value);
      return true;
    }
    return false;
  }

 private:
  SpecificationPtr spec_;
};
}  // namespace trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
