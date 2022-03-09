// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd

#include <pybind11/stl.h>

#include <openassetio/specification/Specification.hpp>

#include "../_openassetio.hpp"

void registerSpecification(const py::module& mod) {
  using openassetio::specification::Specification;

  auto cls = py::class_<Specification, Holder<Specification>>(mod, "Specification")
                 .def(py::init<const Specification::TraitIds&>())
                 .def("hasTrait", &Specification::hasTrait)
                 .def("setTraitProperty", &Specification::setTraitProperty)
                 .def("getTraitProperty", &Specification::getTraitProperty);
}
