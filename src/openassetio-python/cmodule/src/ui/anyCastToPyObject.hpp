// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <any>

#include <fmt/format.h>
#include <pybind11/pybind11.h>

#include <openassetio/errors/exceptions.hpp>

namespace py = pybind11;

inline py::object anyCastToPyObject(const std::any& wrapped) {
  // Plugins should bundle a CPython PyObject* in their std::any.
  if (wrapped.type() == typeid(PyObject*)) {
    return py::reinterpret_borrow<py::object>(std::any_cast<PyObject*>(wrapped));
  }
  // Middleware (e.g. UIDelegate) will forward Python arguments as a
  // py::object bundled in the std::any.
  if (wrapped.type() == typeid(py::object)) {
    return std::any_cast<py::object>(wrapped);
  }
  throw openassetio::errors::InputValidationException{
      fmt::format("Python UI delegates only accept Python objects: C++ type '{}' is not supported",
                  py::detail::clean_type_id(wrapped.type().name()))};
}