// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <openassetio/log/LoggerInterface.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/hostApi/UIDelegateImplementationFactoryInterface.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>  // NOLINT(*-include-cleaner)
#include <openassetio/ui/pluginSystem/CppPluginSystemUIDelegateImplementationFactory.hpp>

#include "../../_openassetio.hpp"

void registerCppPluginSystemUIDelegateImplementationFactory(const py::module_& mod) {
  using openassetio::log::LoggerInterfacePtr;
  using openassetio::ui::hostApi::UIDelegateImplementationFactoryInterface;
  using openassetio::ui::pluginSystem::CppPluginSystemUIDelegateImplementationFactory;

  py::class_<CppPluginSystemUIDelegateImplementationFactory,
             UIDelegateImplementationFactoryInterface,
             CppPluginSystemUIDelegateImplementationFactory::Ptr>(
      mod, "CppPluginSystemUIDelegateImplementationFactory", py::is_final())
      .def_readonly_static("kPluginEnvVar",
                           &CppPluginSystemUIDelegateImplementationFactory::kPluginEnvVar)
      .def(py::init(
               RetainCommonPyArgs::forFn<py::overload_cast<openassetio::Str, LoggerInterfacePtr>(
                   &CppPluginSystemUIDelegateImplementationFactory::make)>()),
           py::arg("paths"), py::arg("logger").none(false))
      .def(py::init(RetainCommonPyArgs::forFn<py::overload_cast<LoggerInterfacePtr>(
                        &CppPluginSystemUIDelegateImplementationFactory::make)>()),
           py::arg("logger").none(false))
      .def("identifiers", &CppPluginSystemUIDelegateImplementationFactory::identifiers,
           py::call_guard<py::gil_scoped_release>{})
      .def("instantiate", &CppPluginSystemUIDelegateImplementationFactory::instantiate,
           py::arg("identifier"), py::call_guard<py::gil_scoped_release>{});
}
