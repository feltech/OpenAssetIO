// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <fmt/format.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <openassetio/Context.hpp>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/hostApi/EntityReferencePager.hpp>
#include <openassetio/managerApi/EntityReferencePagerInterface.hpp>
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

#include "../../_openassetio.hpp"
#include "../../overrideMacros.hpp"
#include "../anyCastToPyObject.hpp"

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::managerApi {
/**
 * Trampoline class required for pybind to bind pure virtual methods
 * and allow C++ -> Python calls via a C++ instance.
 */
struct PyUIDelegateInterface : UIDelegateInterface {
  using UIDelegateInterface::UIDelegateInterface;

  [[nodiscard]] Identifier identifier() const override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(Identifier, UIDelegateInterface, identifier, /* no args */);
  }

  [[nodiscard]] Str displayName() const override {
    OPENASSETIO_PYBIND11_OVERRIDE_PURE(Str, UIDelegateInterface, displayName, /* no args */);
  }

  [[nodiscard]] InfoDictionary info() override {
    OPENASSETIO_PYBIND11_OVERRIDE(InfoDictionary, UIDelegateInterface, info, /* no args */);
  }

  [[nodiscard]] InfoDictionary settings(const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(InfoDictionary, UIDelegateInterface, settings, hostSession);
  }

  void initialize(InfoDictionary uiDelegateSettings, const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateInterface, initialize,
                                  std::move(uiDelegateSettings), hostSession);
  }

  void flushCaches(const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateInterface, flushCaches, hostSession);
  }
  std::any populateUI(const std::any& container, const trait::TraitsDataConstPtr& uiTraits,
                      const trait::TraitsDataConstPtr& entityTraits, const std::any& nativeData,
                      const HostSessionPtr& hostSession) override {
    py::object pyContainer = anyCastToPyObject(container);
    py::object pyNativeData = anyCastToPyObject(nativeData);

    OPENASSETIO_PYBIND11_OVERRIDE_ARGS(
        py::object, UIDelegateInterface, populateUI,
        (container, uiTraits, entityTraits, nativeData, hostSession), std::move(pyContainer),
        uiTraits, entityTraits, std::move(pyNativeData), hostSession);
  }
};

}  // namespace ui::managerApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio

void registerUIDelegateInterface(const py::module& mod) {
  using openassetio::ContextConstPtr;
  using openassetio::EntityReference;
  using openassetio::EntityReferences;
  using openassetio::managerApi::HostSessionPtr;
  using openassetio::trait::TraitsDataConstPtr;
  using openassetio::trait::TraitsDataPtr;
  using openassetio::ui::managerApi::PyUIDelegateInterface;
  using openassetio::ui::managerApi::UIDelegateInterface;
  using openassetio::ui::managerApi::UIDelegateInterfacePtr;

  py::class_<UIDelegateInterface, PyUIDelegateInterface, UIDelegateInterfacePtr>
      pyUIDelegateInterface(mod, "UIDelegateInterface");

  pyUIDelegateInterface.def(py::init())
      .def("identifier", &UIDelegateInterface::identifier,
           py::call_guard<py::gil_scoped_release>{})
      .def("displayName", &UIDelegateInterface::displayName,
           py::call_guard<py::gil_scoped_release>{})
      .def("info", &UIDelegateInterface::info, py::call_guard<py::gil_scoped_release>{})
      .def("settings", &UIDelegateInterface::settings, py::arg("hostSession").none(false),
           py::call_guard<py::gil_scoped_release>{})
      .def("initialize", &UIDelegateInterface::initialize, py::arg("managerSettings"),
           py::arg("hostSession").none(false), py::call_guard<py::gil_scoped_release>{})
      .def("flushCaches", &UIDelegateInterface::flushCaches, py::arg("hostSession").none(false),
           py::call_guard<py::gil_scoped_release>{})
      .def(
          "populateUI",
          [](UIDelegateInterface& self, const py::object& container,
             const TraitsDataConstPtr& uiTraits, const TraitsDataConstPtr& entityTraits,
             const py::object& nativeData, const HostSessionPtr& hostSession) {
            const std::any result =
                self.populateUI(container, uiTraits, entityTraits, nativeData, hostSession);
            return anyCastToPyObject(result);
          },
          py::arg("container"), py::arg("uiTraits").none(false),
          py::arg("entityTraits").none(false), py::arg("nativeData"),
          py::arg("hostSession").none(false), py::call_guard<py::gil_scoped_release>{});
}