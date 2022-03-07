// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#pragma once

#include <memory>

#include <pybind11/pybind11.h>

namespace py = pybind11;

template <class T>
using Holder = std::shared_ptr<T>;

void registerManagerInterface(const py::module& mod);
void registerSpecificationData(const py::module& mod);
