// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <vector>

#include <pybind11/pybind11.h>

#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

struct ReturnNonPyObjFromPopulateUI : openassetio::ui::managerApi::UIDelegateInterface {
  [[nodiscard]] openassetio::Identifier identifier() const override {
    return "org.openassetio.test.stub.uiDelegate.cpp";
  }
  [[nodiscard]] openassetio::Str displayName() const override { return "Stub C++ UI Delegate"; }

  std::any populateUI([[maybe_unused]] const std::any& container,
                      // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
                      [[maybe_unused]] const openassetio::trait::TraitsDataConstPtr& uiTraits,
                      [[maybe_unused]] const openassetio::trait::TraitsDataConstPtr& entityTraits,
                      [[maybe_unused]] const std::any& nativeData,
                      [[maybe_unused]] const HostSessionPtr& hostSession) override {
    // Return a non-PyObject to test error handling.
    return float{};
  };
};

struct ReturnRawCPythonFromPopulateUI : openassetio::ui::managerApi::UIDelegateInterface {
  [[nodiscard]] openassetio::Identifier identifier() const override {
    return "org.openassetio.test.stub.uiDelegate.cpp";
  }
  [[nodiscard]] openassetio::Str displayName() const override { return "Stub C++ UI Delegate"; }

  std::any populateUI([[maybe_unused]] const std::any& container,
                      // NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
                      [[maybe_unused]] const openassetio::trait::TraitsDataConstPtr& uiTraits,
                      [[maybe_unused]] const openassetio::trait::TraitsDataConstPtr& entityTraits,
                      [[maybe_unused]] const std::any& nativeData,
                      [[maybe_unused]] const HostSessionPtr& hostSession) override {
    // Return a raw CPython object to test conversion.
    return PyLong_FromLong(42);  // NOLINT(readability-magic-numbers)
  };
};

namespace py = pybind11;

void registerUIDelegateTestUtils(py::module& mod) {
  using openassetio::trait::TraitsData;
  using openassetio::ui::hostApi::UIDelegatePtr;
  using openassetio::ui::managerApi::UIDelegateInterfacePtr;

  auto ui = mod.def_submodule("ui");

  ui.def(
      "createUIDelegateInterfaceThatReturnsNonPyObjFromPopulateUI",
      []() -> UIDelegateInterfacePtr { return std::make_shared<ReturnNonPyObjFromPopulateUI>(); });

  ui.def("createUIDelegateInterfaceThatReturnsRawCPythonFromPopulateUI",
         []() -> UIDelegateInterfacePtr {
           return std::make_shared<ReturnRawCPythonFromPopulateUI>();
         });

  ui.def("callPopulateUIWithNonPyObjectContainer", [](const UIDelegatePtr& uiDelegate) {
    uiDelegate->populateUI(int{}, TraitsData::make(), TraitsData::make(),
                           static_cast<py::object&&>(py::none{}));
  });

  ui.def("callPopulateUIWithNonPyObjectNativeData", [](const UIDelegatePtr& uiDelegate) {
    uiDelegate->populateUI(static_cast<py::object&&>(py::none{}), TraitsData::make(),
                           TraitsData::make(), double{});
  });
}
