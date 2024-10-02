// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <algorithm>
#include <utility>

#include <pybind11/attr.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <openassetio/Context.hpp>
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/ui/UIDelegateState.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

#include "../../_openassetio.hpp"
#include "openassetio/trait/TraitsData.hpp"

void registerUIDelegate(const py::module& mod) {
  using openassetio::ContextConstPtr;
  using openassetio::trait::TraitsDataConstPtr;
  using openassetio::ui::hostApi::UIDelegate;
  using DispatchStateCallback = openassetio::ui::hostApi::UIDelegate::DispatchStateCallback;
  using openassetio::ui::UIDelegateState;
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
      .def("flushCaches", &UIDelegate::flushCaches, py::call_guard<py::gil_scoped_release>{})
      .def(
          "populateUI",
          [](UIDelegate& self, const TraitsDataConstPtr& uiTraits, UIDelegateState initialState,
             const ContextConstPtr& context, DispatchStateCallback stateChangedCallback) {
            return self.populateUI(uiTraits, std::move(initialState), context,
                                   std::move(stateChangedCallback));
          },
          py::arg("container"), py::arg("uiTraits").none(false),
          py::arg("entityTraits").none(false), py::arg("nativeData"),
          py::call_guard<py::gil_scoped_release>{});
}  // NOLINT(readability/fn_size)
