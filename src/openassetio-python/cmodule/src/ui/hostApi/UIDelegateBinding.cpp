// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <algorithm>

#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

#include "../../_openassetio.hpp"

void registerUIDelegate(const py::module& mod) {
  using UIDelegate = openassetio::ui::hostApi::UIDelegate;
  py::class_<UIDelegate, UIDelegate::Ptr> pyUIDelegate{mod, "UIDelegate", py::is_final()};

  pyUIDelegate
      .def(py::init(RetainCommonPyArgs::forFn<&UIDelegate::make>()),
           py::arg("uiDelegateInterface").none(false), py::arg("hostSession").none(false))
      .def("identifier", &UIDelegate::identifier, py::call_guard<py::gil_scoped_release>{})
      .def("displayName", &UIDelegate::displayName, py::call_guard<py::gil_scoped_release>{})
      .def("info", &UIDelegate::info, py::call_guard<py::gil_scoped_release>{})
      .def("settings", &UIDelegate::settings, py::call_guard<py::gil_scoped_release>{})
      .def("initialize", &UIDelegate::initialize, py::arg("uiDelegateSettings"),
           py::call_guard<py::gil_scoped_release>{})
      .def("flushCaches", &UIDelegate::flushCaches, py::call_guard<py::gil_scoped_release>{});
}  // NOLINT(readability/fn_size)
