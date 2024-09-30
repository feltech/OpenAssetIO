// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <pybind11/pybind11.h>

namespace py = pybind11;

void registerPyRetainingSharedPtrTestTypes(py::module_&);
void registerExceptionThrower(py::module_& mod);
void registerRunInThread(py::module_& mod);
void registerUIDelegateTestUtils(py::module& mod);

void registerTestUtils(py::module& mod) {
  py::module_ testutils = mod.def_submodule("_testutils");
  registerPyRetainingSharedPtrTestTypes(testutils);
  registerExceptionThrower(testutils);
  registerRunInThread(testutils);
  registerUIDelegateTestUtils(testutils);
}
