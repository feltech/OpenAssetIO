// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Provide the base dynamic specification class.
 */
#pragma once

#include <memory>
#include <vector>

#include <openassetio/export.h>

#include "../trait/property.hpp"

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
/**
 * Comprises the base Specification class and concrete classes of core
 * specifications derived from it.
 */
namespace specification {

/**
 * Base specification class that is defined by a given list of trait
 * IDs.
 */
class OPENASSETIO_CORE_EXPORT Specification {
 public:
  /// List of supported trait IDs.
  using TraitIds = std::vector<trait::TraitId>;

  /**
   * Construct such that this specification supports the given list of
   * supported trait IDs.
   *
   * @param traitIds List of IDs of traits that this specification
   * supports.
   */
  explicit Specification(const TraitIds& traitIds);

  /**
   * Defaulted virtual destructor establishing this as a base class
   * for more concrete specifications.
   */
  virtual ~Specification();

  /**
   * Return whether this specification supports the given trait.
   *
   * @param traitId ID of trait to check for support.
   * @return `true` if trait is supported, `false` otherwise.
   */
  [[nodiscard]] bool hasTrait(const trait::TraitId& traitId) const;

  /**
   * Get the value of a given trait property, if the property has
   * been set.
   *
   * @param[out] out Storage fo result, only written to if the property
   * is set.
   * @param traitId ID of trait to query.
   * @param propertyKey Key of trait's property to query.
   * @return `true` if value was found, `false` if it is unset.
   * @exception `std::out_of_range` if the trait is not supported by
   * this specification.
   */
  [[nodiscard]] bool getTraitProperty(trait::property::Value* out, const trait::TraitId& traitId,
                                      const trait::property::Key& propertyKey) const;

  /**
   * Set the value of given trait property.
   *
   * @param traitId ID of trait to update.
   * @param propertyKey Key of property to set.
   * @param propertyValue Value to set.
   * @exception `std::out_of_range` if the trait is not supported by
   * this specification.
   */
  void setTraitProperty(const trait::TraitId& traitId, const trait::property::Key& propertyKey,
                        trait::property::Value propertyValue);

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};
}  // namespace specification
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
