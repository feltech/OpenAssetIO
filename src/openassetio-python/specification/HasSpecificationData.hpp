// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
/**
 * Provide functions for registering HasSpecificationData with Python.
 */
#pragma once

#include <pybind11/pybind11.h>

#include <openassetio/specification/SpecificationData.hpp>

/// Concise pybind alias.
namespace py = pybind11;

/// Register abstract base HasSpecificationData members on a subclass.
template <class Class, typename... Rest>
void registerHasSpecificationData(py::class_<Class, Rest...>& cls) {
  cls.def("data", [](Class& self) { return self.data(); });
}
