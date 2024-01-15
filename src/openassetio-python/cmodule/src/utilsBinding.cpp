// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <openassetio/utils/path.hpp>

#include "_openassetio.hpp"

void registerUtils(py::module_ &mod) {
  namespace utils = openassetio::utils;

  py::enum_<utils::PathType>{mod, "PathType"}
      .value("kSystem", utils::PathType::kSystem)
      .value("kPOSIX", utils::PathType::kPOSIX)
      .value("kWindows", utils::PathType::kWindows);

  mod.def("pathToUrl", &utils::pathToUrl, py::arg("absolutePath"),
          py::arg("pathType") = utils::PathType::kSystem)
      .def("pathFromUrl", &utils::pathFromUrl, py::arg("fileUrl"),
           py::arg("pathType") = utils::PathType::kSystem);
}
