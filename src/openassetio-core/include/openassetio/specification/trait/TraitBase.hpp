// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Base class for all specification traits.
 */
#pragma once
#include <algorithm>
#include <memory>
#include <utility>

#include "../Specification.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace specification::trait {

/**
 * Abstract CRTP base class for specification traits.
 *
 * @tparam Derived Concrete subclass.
 */
template <class Derived>
struct TraitBase {
  using SpecificationPtr = std::shared_ptr<Specification>;

  explicit TraitBase(SpecificationPtr spec) : spec_{std::move(spec)} {}

  [[nodiscard]] static const TraitId& traitId() { return Derived::kId; }
  [[nodiscard]] bool isValid() const { return spec_->hasTrait(traitId()); }

 protected:
  SpecificationPtr& spec() { return spec_; }
  [[nodiscard]] const SpecificationPtr& spec() const { return spec_; }

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
}  // namespace specification::trait
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
