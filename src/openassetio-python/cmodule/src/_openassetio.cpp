// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2024 The Foundry Visionmongers Ltd

#include "_openassetio.hpp"

#ifdef OPENASSETIO_ENABLE_TESTS
// In order to have representative tests of internal functionality, we
// must augment this extension module with test-specific bindings. The
// static library providing the implementation of this entry point is
// conditionally linked into this extension module.
void registerTestUtils(py::module& mod);
#endif

PYBIND11_MODULE(_openassetio, mod) {
  namespace py = pybind11;

  // Note: the `register` functions here should be called in dependency
  // order. E.g. `Manager` depends on `ManagerInterface`, so
  // `registerManagerInterface` should be called first. This is so
  // pybind11 will properly report type names in its docstring/error
  // output.

  const py::module access = mod.def_submodule("access");
  const py::module managerApi = mod.def_submodule("managerApi");
  const py::module hostApi = mod.def_submodule("hostApi");
  const py::module log = mod.def_submodule("log");
  const py::module constants = mod.def_submodule("constants");
  const py::module errors = mod.def_submodule("errors");
  const py::module trait = mod.def_submodule("trait");
  py::module utils = mod.def_submodule("utils");
  const py::module pluginSystem = mod.def_submodule("pluginSystem");

  registerVersion(mod);
  registerAccess(access);
  registerConstants(constants);
  registerLoggerInterface(log);
  registerConsoleLogger(log);
  registerSeverityFilter(log);
  registerTraitsData(trait);
  registerManagerStateBase(managerApi);
  registerContext(mod);
  registerBatchElementError(errors);
  registerExceptions(errors);
  registerEntityReference(mod);
  registerManagerUIBase(managerApi);
  registerHostInterface(hostApi);
  registerHost(managerApi);
  registerHostSession(managerApi);
  registerEntityReferencePagerInterface(managerApi);
  registerEntityReferencePager(hostApi);
  registerManagerInterface(managerApi);
  registerManagerImplementationFactoryInterface(hostApi);
  registerManager(hostApi);
  registerManagerFactory(hostApi);
  registerUtils(utils);
  registerCppPluginSystemPlugin(pluginSystem);
  registerCppPluginSystem(pluginSystem);
  registerCppPluginSystemManagerImplementationFactory(pluginSystem);

#ifdef OPENASSETIO_ENABLE_TESTS
  registerTestUtils(mod);
#endif
}
