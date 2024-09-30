// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <openassetio/ui/constants.hpp>

#include "../_openassetio.hpp"

void registerUIConstants(const py::module_ &mod) {
  mod.attr("kInfoKey_Icon") = openassetio::ui::constants::kInfoKey_Icon;
  mod.attr("kInfoKey_SmallIcon") = openassetio::ui::constants::kInfoKey_SmallIcon;
  mod.attr("kInfoKey_PythonOnly") = openassetio::ui::constants::kInfoKey_PythonOnly;
}
