// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd

#include "_openassetio.hpp"

PYBIND11_MODULE(_openassetio, mod) {
  namespace py = pybind11;

  py::module managerAPI = mod.def_submodule("managerAPI");

  registerManagerInterface(managerAPI);

  py::module specification = mod.def_submodule("specification");

  registerSpecificationData(specification);
  registerSpecificationBase(specification);

  py::module trait = specification.def_submodule("trait");

  registerBlobTrait(trait);
  registerBlobSpecification(specification);
}
