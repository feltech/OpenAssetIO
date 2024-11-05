// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <memory>

#include <pybind11/pybind11.h>

#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/UIDelegateState.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

namespace py = pybind11;

void registerUIDelegateTestUtils(py::module& mod) {
  using openassetio::trait::TraitsData;
  using openassetio::ui::UIDelegateRequest;
  using openassetio::ui::hostApi::UIDelegatePtr;
  using openassetio::ui::managerApi::UIDelegateInterfacePtr;

  auto ui = mod.def_submodule("ui");

  ui.def("createUIDelegateStateWithNonPyObjectNativeData", [] {
    auto state = UIDelegateRequest::make();
    state->setNativeData(double{});
    return state;
  });
  ui.def("createUIDelegateStateWithRawCPythonNativeData", [] {
    auto state = UIDelegateRequest::make();
    state->setNativeData(PyLong_FromLong(42));  // NOLINT(readability-magic-numbers)
    return state;
  });
}
