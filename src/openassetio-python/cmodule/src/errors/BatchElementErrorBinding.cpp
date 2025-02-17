// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2025 The Foundry Visionmongers Ltd
#include <sstream>

#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <openassetio/errors/BatchElementError.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/utils/ostream.hpp>

#include "../_openassetio.hpp"

void registerBatchElementError(const py::module& mod) {
  using openassetio::errors::BatchElementError;

  py::class_<BatchElementError> batchElementError{mod, "BatchElementError", py::is_final()};

  py::enum_<BatchElementError::ErrorCode>{batchElementError, "ErrorCode"}
      .value("kUnknown", BatchElementError::ErrorCode::kUnknown)
      .value("kInvalidEntityReference", BatchElementError::ErrorCode::kInvalidEntityReference)
      .value("kMalformedEntityReference", BatchElementError::ErrorCode::kMalformedEntityReference)
      .value("kEntityAccessError", BatchElementError::ErrorCode::kEntityAccessError)
      .value("kEntityResolutionError", BatchElementError::ErrorCode::kEntityResolutionError)
      .value("kInvalidPreflightHint", BatchElementError::ErrorCode::kInvalidPreflightHint)
      .value("kInvalidTraitSet", BatchElementError::ErrorCode::kInvalidTraitSet)
      .value("kAuthError", BatchElementError::ErrorCode::kAuthError);

  batchElementError
      .def(py::init<BatchElementError::ErrorCode, openassetio::Str>(), py::arg("code"),
           py::arg("message"))
      .def(py::self == py::self)  // NOLINT(misc-redundant-expression)
      .def_readonly("code", &BatchElementError::code)
      .def_readonly("message", &BatchElementError::message)
      .def("__str__", [](const BatchElementError& self) {
        std::ostringstream stringStream;
        stringStream << self;
        return stringStream.str();
      });
}
