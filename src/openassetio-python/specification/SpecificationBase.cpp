// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <pybind11/stl.h>

#include <openassetio/specification/SpecificationData.hpp>
#include <openassetio/specification/SpecificationBase.hpp>

#include "../_openassetio.hpp"
#include "HasSpecificationData.hpp"

void registerSpecificationBase(const py::module& mod) {
  using openassetio::specification::SpecificationBase;
  using openassetio::specification::HasSpecificationData;

  // TODO(DF): Enable __init__ and add trampoline class for calling up
  //  to Python subclass.
  auto cls = py::class_<SpecificationBase, Holder<SpecificationBase>>(mod, "SpecificationBase")
      .def("traitIDs", &SpecificationBase::traitIDs);
  registerHasSpecificationData(cls);
}
