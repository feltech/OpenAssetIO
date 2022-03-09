// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <optional>

#include <pybind11/stl.h>

#include <openassetio/specification/Specification.hpp>

#include "../_openassetio.hpp"

void registerSpecification(const py::module& mod) {
  using openassetio::specification::Specification;
  namespace trait = openassetio::specification::trait;
  namespace property = openassetio::specification::trait::property;
  using MaybeValue = std::optional<property::Value>;

  auto cls = py::class_<Specification, Holder<Specification>>(mod, "Specification")
                 .def(py::init<const Specification::TraitIds&>())
                 .def("hasTrait", &Specification::hasTrait)
                 .def("setTraitProperty", &Specification::setTraitProperty)
                 .def("getTraitProperty",
                      [](const Specification& self, const trait::TraitId& traitId,
                         const property::Key& key) -> MaybeValue {
                        if (property::Value out; self.getTraitProperty(&out, traitId, key)) {
                          return out;
                        }
                        return {};
                      });
}
