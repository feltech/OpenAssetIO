// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once

#include <any>
#include <string>

#include <fmt/format.h>
#include <pybind11/pybind11.h>

#include <openassetio/errors/exceptions.hpp>

namespace py = pybind11;

inline py::object anyCastToPyObject(const std::any &wrapped) {
  if (!wrapped.has_value()) {
    return py::none{};
  }
  // Hosts, managers and middleware should bundle a CPython PyObject* in
  // their std::any.
  if (wrapped.type() == typeid(PyObject *)) {
    return py::reinterpret_borrow<py::object>(std::any_cast<PyObject *>(wrapped));
  }
  std::string wrappedTypeName = wrapped.type().name();
  py::detail::clean_type_id(wrappedTypeName);
  throw openassetio::errors::InputValidationException{
      fmt::format("Python UI delegates only accept Python objects: C++ type '{}' is not supported",
                  wrappedTypeName)};
}
