// SPDX-License-Identifier: Apache-2.0
// Copyright 2025 The Foundry Visionmongers Ltd

#include <any>
#include <string>

#include <fmt/core.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <openassetio/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/ui/UIDelegateStateInterface.hpp>

#include "../_openassetio.hpp"
#include "../overrideMacros.hpp"

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui {

/**
 * pybind11 trampoline class to call Python subclass implementation.
 *
 * Ensure the nativeData() implementation always stores a PyObject*.
 *
 * Note the py::handle -> std::any in pyNativeData(), followed by
 * the std::any -> py::handle -> PyObject* -> std::any in nativeData().
 *
 * This dance is required so we can make use of the PYBIND11 macros.
 */
struct PyUIDelegateRequestInterface : UIDelegateRequestInterface {
  [[nodiscard]] std::any nativeData() override {
    return std::any_cast<py::handle>(pyNativeData()).ptr();
  }

  [[nodiscard]] std::any pyNativeData() {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(py::handle, UIDelegateRequestInterface, nativeData,
                                       /* no args */);
  }

  [[nodiscard]] EntityReferences entityReferences() override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(
        EntityReferences, UIDelegateRequestInterface, entityReferences, /* no args */);
  }

  [[nodiscard]] trait::TraitsDatas entityTraitsDatas() override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(
        trait::TraitsDatas, UIDelegateRequestInterface, entityTraitsDatas, /* no args */);
  }

  [[nodiscard]] StateChangedCallback stateChangedCallback() override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(
        StateChangedCallback, UIDelegateRequestInterface, stateChangedCallback, /* no args */);
  }
};

/**
 * pybind11 trampoline class to call Python subclass implementation.
 *
 * Ensure the nativeData() implementation always stores a PyObject*.
 *
 * Note the py::handle -> std::any in pyNativeData(), followed by
 * the std::any -> py::handle -> PyObject* -> std::any in nativeData().
 *
 * This dance is required so we can make use of the PYBIND11 macros.
 */
struct PyUIDelegateStateInterface : UIDelegateStateInterface {
  [[nodiscard]] std::any nativeData() override {
    return std::any_cast<py::handle>(pyNativeData()).ptr();
  }
  [[nodiscard]] std::any pyNativeData() {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(py::handle, UIDelegateStateInterface, nativeData,
                                       /* no args */);
  }
  [[nodiscard]] EntityReferences entityReferences() override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(
        EntityReferences, UIDelegateStateInterface, entityReferences, /* no args */);
  }

  [[nodiscard]] trait::TraitsDatas entityTraitsDatas() override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(
        trait::TraitsDatas, UIDelegateStateInterface, entityTraitsDatas, /* no args */);
  }

  [[nodiscard]] UpdateRequestCallback updateRequestCallback() override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(
        UpdateRequestCallback, UIDelegateStateInterface, updateRequestCallback, /* no args */);
  }
};

}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio

namespace {

/**
 * Extract a PyObject* from a std::any, and throw an error if the
 * std::any does not contain a PyObject*.
 */
py::object anyCastToPyObject(const std::any& wrapped) {
  if (!wrapped.has_value()) {
    return py::none{};
  }

  // Hosts, managers and middleware should bundle a CPython PyObject* in
  // their std::any.
  if (wrapped.type() == typeid(PyObject*)) {
    return py::reinterpret_borrow<py::object>(std::any_cast<PyObject*>(wrapped));
  }

  // Use pybind11 utility to demangle the typeid.
  std::string wrappedTypeName = wrapped.type().name();
  py::detail::clean_type_id(wrappedTypeName);

  throw openassetio::errors::InputValidationException{
      fmt::format("Python UI delegates only accept Python objects: C++ type '{}' is not supported",
                  wrappedTypeName)};
}
}  // namespace

void registerUIDelegateState(const py::module& mod) {
  using openassetio::EntityReferences;
  using openassetio::trait::TraitsDataPtr;
  using openassetio::trait::TraitsDatas;
  using openassetio::ui::PyUIDelegateRequestInterface;
  using openassetio::ui::PyUIDelegateStateInterface;
  using openassetio::ui::UIDelegateRequestInterface;
  using openassetio::ui::UIDelegateRequestInterfacePtr;
  using openassetio::ui::UIDelegateStateInterface;
  using openassetio::ui::UIDelegateStateInterfacePtr;

  // Note: we "forward declare" the classes, since they have a circular
  // dependency, and pybind11 needs the types available to calculate
  // docstrings.
  py::class_<UIDelegateRequestInterface, PyUIDelegateRequestInterface,
             UIDelegateRequestInterfacePtr>
      pyUIDelegateRequestInterface(mod, "UIDelegateRequestInterface");
  py::class_<UIDelegateStateInterface, PyUIDelegateStateInterface, UIDelegateStateInterfacePtr>
      pyUIDelegateStateInterface(mod, "UIDelegateStateInterface");

  pyUIDelegateRequestInterface.def(py::init())
      .def("nativeData",
           [](UIDelegateRequestInterface& self) { return anyCastToPyObject(self.nativeData()); })
      .def("entityReferences", &UIDelegateRequestInterface::entityReferences)
      .def("entityTraitsDatas", &UIDelegateRequestInterface::entityTraitsDatas)
      .def("stateChangedCallback", &UIDelegateRequestInterface::stateChangedCallback);

  pyUIDelegateStateInterface.def(py::init())
      .def("nativeData",
           [](UIDelegateStateInterface& self) { return anyCastToPyObject(self.nativeData()); })
      .def("entityReferences", &UIDelegateStateInterface::entityReferences)
      .def("entityTraitsDatas", &UIDelegateStateInterface::entityTraitsDatas)
      .def("updateRequestCallback", &UIDelegateStateInterface::updateRequestCallback);
}
