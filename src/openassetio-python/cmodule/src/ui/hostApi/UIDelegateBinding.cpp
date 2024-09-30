// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <openassetio/Context.hpp>
#include <openassetio/managerApi/HostSession.hpp>  // NOLINT(*-include-cleaner)
#include <openassetio/ui/UIDelegateState.hpp>      // NOLINT(*-include-cleaner)
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

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
           py::call_guard<py::gil_scoped_release>{})
      .def("close", &UIDelegate::close, py::call_guard<py::gil_scoped_release>{})
      .def("flushCaches", &UIDelegate::flushCaches, py::call_guard<py::gil_scoped_release>{})
      .def("uiPolicy", &UIDelegate::uiPolicy, py::arg("uiTraits"), py::arg("uiAccess"),
           py::arg("context").none(false), py::call_guard<py::gil_scoped_release>{})
      .def("populateUI", RetainCommonPyArgs::forFn<&UIDelegate::populateUI>(),
           py::arg("uiTraitsData"), py::arg("uiAccess"), py::arg("requestState"),
           py::arg("context"), py::call_guard<py::gil_scoped_release>{});
}
