// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <utility>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <openassetio/export.h>
#include <openassetio/Context.hpp>
#include <openassetio/EntityReference.hpp>
#include <openassetio/InfoDictionary.hpp>
#include <openassetio/managerApi/EntityReferencePagerInterface.hpp>
#include <openassetio/managerApi/HostSession.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/typedefs.hpp>
#include <openassetio/ui/UIDelegateState.hpp>
#include <openassetio/ui/access.hpp>
#include <openassetio/ui/constants.hpp>
#include <openassetio/ui/managerApi/UIDelegateInterface.hpp>

#include "../../PyRetainingSharedPtr.hpp"
#include "../../_openassetio.hpp"
#include "../../overrideMacros.hpp"

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
    auto infoDict = pyInfo();
    // Enforce the "Python only" flag, so hosts can detect that
    // PyObject* is required in nativeData.
    infoDict[Str{constants::kInfoKey_PythonOnly}] = true;
    return infoDict;
  }

  [[nodiscard]] InfoDictionary pyInfo() {
    OPENASSETIO_PYBIND11_OVERRIDE(InfoDictionary, UIDelegateInterface, info, /* no args */);
  }

  [[nodiscard]] InfoDictionary settings(const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(InfoDictionary, UIDelegateInterface, settings, hostSession);
  }

  void initialize(InfoDictionary uiDelegateSettings, const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateInterface, initialize,
                                  std::move(uiDelegateSettings), hostSession);
  }

  void close(const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateInterface, close, hostSession);
  }

  void flushCaches(const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateInterface, flushCaches, hostSession);
  }
  [[nodiscard]] trait::TraitsDataPtr uiPolicy(const trait::TraitSet& uiTraits,
                                              access::UIAccess uiAccess,
                                              const ContextConstPtr& context,
                                              const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(trait::TraitsDataPtr, UIDelegateInterface, uiPolicy, uiTraits,
                                  uiAccess, context, hostSession);
  }
  UIDelegateStateInterfacePtr populateUI(const trait::TraitsDataConstPtr& uiTraitsData,
                                         const access::UIAccess uiAccess,
                                         const UIDelegateRequestInterfacePtr& requestState,
                                         const ContextConstPtr& context,
                                         const HostSessionPtr& hostSession) override {
    OPENASSETIO_PYBIND11_OVERRIDE(PyRetainingSharedPtr<UIDelegateStateInterface>,
                                  UIDelegateInterface, populateUI, uiTraitsData, uiAccess,
                                  requestState, context, hostSession);
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
  using openassetio::ui::UIDelegateRequestInterface;
  using openassetio::ui::managerApi::PyUIDelegateInterface;
  using openassetio::ui::managerApi::UIDelegateInterface;
  using openassetio::ui::managerApi::UIDelegateInterfacePtr;

  py::class_<UIDelegateInterface, PyUIDelegateInterface, UIDelegateInterfacePtr>(
      mod, "UIDelegateInterface")
      .def(py::init())
      .def("identifier", &UIDelegateInterface::identifier,
           py::call_guard<py::gil_scoped_release>{})
      .def("displayName", &UIDelegateInterface::displayName,
           py::call_guard<py::gil_scoped_release>{})
      .def("info", &UIDelegateInterface::info, py::call_guard<py::gil_scoped_release>{})
      .def("settings", &UIDelegateInterface::settings, py::arg("hostSession").none(false),
           py::call_guard<py::gil_scoped_release>{})
      .def("initialize", &UIDelegateInterface::initialize, py::arg("managerSettings"),
           py::arg("hostSession").none(false), py::call_guard<py::gil_scoped_release>{})
      .def("close", &UIDelegateInterface::close, py::arg("hostSession").none(false),
           py::call_guard<py::gil_scoped_release>{})
      .def("flushCaches", &UIDelegateInterface::flushCaches, py::arg("hostSession").none(false),
           py::call_guard<py::gil_scoped_release>{})
      .def("uiPolicy", &UIDelegateInterface::uiPolicy, py::arg("uiTraits"), py::arg("uiAccess"),
           py::arg("context").none(false), py::arg("hostSession").none(false),
           py::call_guard<py::gil_scoped_release>{})
      .def("populateUI", RetainCommonPyArgs::forFn<&UIDelegateInterface::populateUI>(),
           py::arg("uiTraitsData").none(false), py::arg("uiAccess"), py::arg("requestState"),
           py::arg("context").none(false), py::arg("hostSession").none(false),
           py::call_guard<py::gil_scoped_release>{});
}
