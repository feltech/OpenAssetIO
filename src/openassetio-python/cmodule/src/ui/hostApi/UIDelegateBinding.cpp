// SPDX-License-Identifier: Apache-2.0
// Copyright 2024-2025 The Foundry Visionmongers Ltd
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <openassetio/Context.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>

// NOLINTBEGIN(*-include-cleaner): Needed by pybind11.
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>
// NOLINTEND(*-include-cleaner)

#include "../../_openassetio.hpp"
#include "openassetio/trait/TraitsData.hpp"

void registerUIDelegate(const py::module& mod) {
  using openassetio::ContextConstPtr;
  using openassetio::trait::TraitsDataConstPtr;
  using openassetio::ui::hostApi::UIDelegate;

  py::class_<UIDelegate, UIDelegate::Ptr>{mod, "UIDelegate", py::is_final()}
      .def(py::init(RetainCommonPyArgs::forFn<&UIDelegate::make>()),
           py::arg("uiDelegateInterface").none(false), py::arg("hostSession").none(false))
      .def("identifier", &UIDelegate::identifier, py::call_guard<py::gil_scoped_release>{})
      .def("displayName", &UIDelegate::displayName, py::call_guard<py::gil_scoped_release>{})
      .def("info", &UIDelegate::info, py::call_guard<py::gil_scoped_release>{})
      .def("settings", &UIDelegate::settings, py::call_guard<py::gil_scoped_release>{})
      .def("initialize", &UIDelegate::initialize, py::arg("uiDelegateSettings"),
           py::call_guard<py::gil_scoped_release>{});
}
