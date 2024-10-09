// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#include <pybind11/operators.h>
#include <pybind11/pybind11.h>

#include <openassetio/constants.hpp>

#include "_openassetio.hpp"

void registerConstants(const py::module_ &mod) {
  mod.attr("kInfoKey_EntityReferencesMatchPrefix") =
      openassetio::constants::kInfoKey_EntityReferencesMatchPrefix;
}
