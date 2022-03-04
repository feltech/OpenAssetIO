// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>
// API export generated header
#include <openassetio/export.h>

namespace openassetio {
inline namespace OPENASSETIO_VERSION {
namespace traits {

namespace property {

using Key = std::string;

using Bool = bool;
using Int = int;
using Float = double;
using Str = std::string;
using Map = std::unordered_map<Key, std::variant<Bool, Int, Float, Str>>;

using Value = std::variant<Bool, Int, Float, Str, Map>;

template <class T>
using Maybe = std::optional<T>;
}  // namespace property

using Properties = std::unordered_map<property::Key, property::Value>;

using TraitId = std::string_view;  // TODO(DF): Is this always safe?

class OPENASSETIO_CORE_EXPORT SpecificationData {
 public:
  SpecificationData() = default;

  [[nodiscard]] std::size_t size() const;

  [[nodiscard]] const Properties& at(const property::Key& key) const;
  [[nodiscard]] Properties& at(const property::Key& key);

  template <class T>
  [[nodiscard]] property::Maybe<T> getTraitProperty(const TraitId traitId,
                                                    const property::Key& propertyKey) {
    const auto traitIter = traitDict_.find(traitId);
    if (traitIter == traitDict_.end()) {
      return {};
    }

    const Properties& props = traitIter->second;
    const auto propIter = props.find(propertyKey);
    if (propIter == props.end()) {
      return {};
    }

    // TODO(DF): Make exception message more friendly.
    return std::get<T>(propIter->second);
  }

  void setTraitProperty(TraitId traitId, const property::Key& propertyKey,
                        property::Value propertyValue);

 private:
  using TraitDict = std::unordered_map<TraitId, Properties>;
  TraitDict traitDict_;
};

using SpecificationDataPtr = std::shared_ptr<SpecificationData>;

class HasSpecificationData {
 public:
  HasSpecificationData() = default;
  explicit HasSpecificationData(SpecificationDataPtr data);
  [[nodiscard]] const SpecificationDataPtr& data() const;
  SpecificationDataPtr& data();

 private:
  SpecificationDataPtr data_;
};

struct OPENASSETIO_CORE_EXPORT BaseSpecification : HasSpecificationData {
  virtual ~BaseSpecification() = default;

  using TraitIds = std::vector<TraitId>;
  [[nodiscard]] virtual const TraitIds& traitIDs() const = 0;
};

template <class Derived>
struct OPENASSETIO_CORE_EXPORT BaseTrait : HasSpecificationData {
  static constexpr TraitId kId = Derived::kId;

  explicit BaseTrait(SpecificationDataPtr specificationData)
      : HasSpecificationData(specificationData) {}

  explicit BaseTrait(const BaseSpecification& spec)
      : HasSpecificationData(specDataForTrait(spec)) {}

  virtual ~BaseTrait() = default;

  [[nodiscard]] SpecificationDataPtr specDataForTrait(const BaseSpecification& spec) const {
    const auto& specTraitIDs = spec.traitIDs();
    if (std::find(specTraitIDs.begin(), specTraitIDs.end(), kId) != specTraitIDs.end()) {
      return spec.data();
    }
    return {};
  }

  [[nodiscard]] TraitId traitId() const { return kId; }
  [[nodiscard]] bool isValid() const { return data().get() != nullptr; }
};

struct OPENASSETIO_CORE_EXPORT BlobTrait : BaseTrait<BlobTrait> {
  static constexpr TraitId kId = "blob";

  using BaseTrait<BlobTrait>::BaseTrait;

  [[nodiscard]] property::Maybe<property::Str> getUrl() const;
  void setUrl(property::Str url);

  [[maybe_unused]] [[nodiscard]] property::Maybe<property::Str> getMimeType() const;
  void setMimeType(property::Str mimeType);
};

struct OPENASSETIO_CORE_EXPORT BlobSpecification : BaseSpecification {
  static const TraitIds kTraitIds;

  [[nodiscard]] const TraitIds& traitIDs() const override;
};

}  // namespace traits
}  // namespace OPENASSETIO_VERSION
}  // namespace openassetio
