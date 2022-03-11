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
   * @param specification Underlying specification to wrap.
   */
  explicit TraitBase(SpecificationPtr specification) : specification_{std::move(specification)} {}

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
  [[nodiscard]] bool isValid() const { return specification_->hasTrait(traitId()); }

 protected:
  /**
   * Get the underlying Specification that this trait is wrapping.
   *
   * @return Wrapped Specification.
   */
  SpecificationPtr& specification() { return specification_; }

  /**
   * Get the underlying Specification that this trait is wrapping.
   *
   * @return Wrapped Specification.
   */
  [[nodiscard]] const SpecificationPtr& specification() const { return specification_; }

  /**
   * Convenience typed accessor to properties in the underlying
   * Specification.
   *
   * @tparam T Type to extract from variant property.
   * @param[out] out Storage for value, if property is set.
   * @param traitId ID of trait to query.
   * @param propertyKey Key of property to query.
   * @return Status of property in the specification.
   */
  template <class T>
  [[nodiscard]] TraitPropertyStatus getTraitProperty(T* out, const TraitId& traitId,
                                                     const property::Key& propertyKey) const {
    if (property::Value value; specification()->getTraitProperty(&value, traitId, propertyKey)) {
      if (T* maybeOut = std::get_if<T>(&value)) {
        *out = *maybeOut;
        return TraitPropertyStatus::kFound;
      }
      return TraitPropertyStatus::kInvalidValue;
    }
    return TraitPropertyStatus::kMissing;
  }

 private:
  SpecificationPtr specification_;
};
}  // namespace trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
