// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <pybind11/stl.h>

#include <openassetio/specification/SpecificationData.hpp>

#include "../_openassetio.hpp"

void registerSpecificationData(const py::module& mod) {
  using openassetio::specification::SpecificationData;
  using openassetio::specification::SpecificationDataPtr;
  namespace trait = openassetio::specification::trait;
  static_assert(std::is_same_v<Holder<SpecificationData>, SpecificationDataPtr>,
                "pybind11 holder type mismatch");

  py::class_<SpecificationData, Holder<SpecificationData>>(mod, "SpecificationData")
      .def(py::init())
      .def("size", &SpecificationData::size)
      .def("getTraitProperty",
           [](const SpecificationData& self, const trait::TraitId& traitId,
              const trait::property::Key& propertyKey) {
             return self.getTraitProperty(traitId, propertyKey);
           })
      .def("setTraitProperty", &SpecificationData::setTraitProperty);
}
