// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <algorithm>
#include <any>
#include <iterator>
#include <utility>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <openassetio/export.h>
#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/ui/UIDelegateState.hpp>

#include "../_openassetio.hpp"
#include "../overrideMacros.hpp"
#include "anyCastToPyObject.hpp"

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui {
struct PyUIDelegateRequest : UIDelegateRequest {
  static UIDelegateRequest::Ptr make() { return std::make_shared<PyUIDelegateRequest>(); }

  [[nodiscard]] UIDelegateRequest::Ptr copy() const override {
    auto other = make();
    copyTo(other);
    auto pySelf = py::cast(this);
    auto pyOther = py::cast(other);
    if (py::hasattr(pySelf, "__nativeData__")) {
      pyOther.attr("__nativeData__") = pySelf.attr("__nativeData__");
    }
    if (py::hasattr(pySelf, "__stateChangedCallback__")) {
      pyOther.attr("__stateChangedCallback__") = pySelf.attr("__stateChangedCallback__");
    }
    return other;
  }

  ~PyUIDelegateRequest() override = default;

  [[nodiscard]] std::any getNativeData() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(PyObject*, UIDelegateRequest, getNativeData);
  }

  [[nodiscard]] py::object getPyNativeData() const { return anyCastToPyObject(getNativeData()); }

  [[nodiscard]] const EntityReferences& getEntityReferences() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(const EntityReferences&, UIDelegateRequest, getEntityReferences);
  }

  [[nodiscard]] const trait::TraitsDatas& getEntityTraitsDatas() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(const trait::TraitsDatas&, UIDelegateRequest,
                                  getEntityTraitsDatas);
  }

  [[nodiscard]] const StateChangedCallback& getStateChangedCallback() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(const StateChangedCallback&, UIDelegateRequest,
                                  getStateChangedCallback);
  }

  void setNativeData(std::any nativeData) override {
    auto pyNativeData = anyCastToPyObject(nativeData);
    py::cast(this).attr("__nativeData__") = nativeData;  // Keep alive.
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateRequest, setNativeData, pyNativeData);
  }

  void setPyNativeData(py::object nativeData) { setNativeData(nativeData.ptr()); }

  void setEntityReferences(EntityReferences entityReferences) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateRequest, setEntityReferences,
                                  std::move(entityReferences));
  }

  void setEntityTraitsDatas(trait::TraitsDatas entityTraitsDatas) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateRequest, setEntityTraitsDatas,
                                  std::move(entityTraitsDatas));
  }

  void setStateChangedCallback(StateChangedCallback callback) override {
    // Place reference on Python wrapper so that garbage collection can
    // resolve circular references (i.e. closures).
    py::object pyCallback = py::cast(callback);
    py::cast(this).attr("__stateChangedCallback__") = pyCallback;
    auto wrapped([weakCallback = py::weakref{py::handle{pyCallback}}](
                     const UIDelegateStateConstPtr& state) { (void)weakCallback()(state); });
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateRequest, setStateChangedCallback, wrapped);
  }
};

struct PyUIDelegateState : UIDelegateState {
  // Override the virtual destructor
  ~PyUIDelegateState() override = default;

  [[nodiscard]] UIDelegateState::Ptr copy() const override {
    auto other = make();
    copyTo(other);
    auto pySelf = py::cast(this);
    auto pyOther = py::cast(other);
    if (py::hasattr(pySelf, "__nativeData__")) {
      pyOther.attr("__nativeData__") = pySelf.attr("__nativeData__");
    }
    if (py::hasattr(pySelf, "__updateRequestCallback__")) {
      pyOther.attr("__updateRequestCallback__") = pySelf.attr("__updateRequestCallback__");
    }
    return other;
  }

  [[nodiscard]] std::any getNativeData() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(PyObject*, UIDelegateState, getNativeData);
  }

  [[nodiscard]] py::object getPyNativeData() const { return anyCastToPyObject(getNativeData()); }

  [[nodiscard]] const EntityReferences& getEntityReferences() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(const EntityReferences&, UIDelegateState, getEntityReferences);
  }

  [[nodiscard]] const trait::TraitsDatas& getEntityTraitsDatas() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(const trait::TraitsDatas&, UIDelegateState,
                                  getEntityTraitsDatas);
  }

  [[nodiscard]] const UpdateRequestCallback& getUpdateRequestCallback() const override {
    OPENASSETIO_PYBIND11_OVERRIDE(const UpdateRequestCallback&, UIDelegateState,
                                  getUpdateRequestCallback);
  }

  void setNativeData(std::any nativeData) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateState, setNativeData, nativeData);
  }

  void setPyNativeData(py::object nativeData) {
    py::cast(this).attr("__nativeData__") = nativeData;  // Keep alive.
    setNativeData(nativeData.ptr());
  }

  void setEntityReferences(EntityReferences entityReferences) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateState, setEntityReferences, entityReferences);
  }

  void setEntityTraitsDatas(trait::TraitsDatas entityTraitsDatas) override {
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateState, setEntityTraitsDatas, entityTraitsDatas);
  }

  void setUpdateRequestCallback(UpdateRequestCallback callback) override {
    // Place reference on Python wrapper so that garbage collection can
    // resolve circular references (i.e. closures).
    py::object pyCallback = py::cast(callback);
    py::cast(this).attr("__updateRequestCallback__") = pyCallback;
    auto wrapped([weakCallback = py::weakref{py::handle{pyCallback}}](
                     const UIDelegateRequestConstPtr& state) { (void)weakCallback()(state); });
    OPENASSETIO_PYBIND11_OVERRIDE(void, UIDelegateState, setUpdateRequestCallback, wrapped);
  }
};

}  // namespace ui
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio

void registerUIDelegateState(const py::module& mod) {
  using openassetio::EntityReferences;
  using openassetio::trait::TraitsDataPtr;
  using openassetio::trait::TraitsDatas;
  using openassetio::ui::PyUIDelegateRequest;
  using openassetio::ui::PyUIDelegateState;
  using openassetio::ui::UIDelegateRequest;
  using openassetio::ui::UIDelegateRequestConstPtr;
  using openassetio::ui::UIDelegateState;
  using openassetio::ui::UIDelegateStateConstPtr;

  py::class_<UIDelegateRequest, PyUIDelegateRequest, UIDelegateRequest::Ptr> pyUIDelegateRequest(
      mod, "UIDelegateRequest", py::dynamic_attr{});
  py::class_<UIDelegateState, PyUIDelegateState, UIDelegateState::Ptr> pyUIDelegateState(
      mod, "UIDelegateState", py::dynamic_attr{});

  pyUIDelegateRequest.def(py::init(&PyUIDelegateRequest::make))
      .def("__copy__", [](const UIDelegateRequest& self) { return self.copy(); })
      .def(
          "__deepcopy__",
          [](const UIDelegateRequest& self, [[maybe_unused]] const py::dict& memo) {
            return static_cast<const PyUIDelegateRequest&>(self).copy();
          },
          py::arg("memo"))
      .def("copy",
           [](const UIDelegateRequest& self) {
             return static_cast<const PyUIDelegateRequest&>(self).copy();
           })
      .def("getNativeData",
           [](const UIDelegateRequest& self) {
             return static_cast<const PyUIDelegateRequest&>(self).getPyNativeData();
           })
      .def("setNativeData",
           [](UIDelegateRequest& self, py::object nativeData) {
             static_cast<PyUIDelegateRequest&>(self).setPyNativeData(std::move(nativeData));
           })
      .def("getEntityReferences", &UIDelegateRequest::getEntityReferences)
      .def("setEntityReferences", &UIDelegateRequest::setEntityReferences)
      .def("getEntityTraitsDatas", &UIDelegateRequest::getEntityTraitsDatas)
      .def("setEntityTraitsDatas", &UIDelegateRequest::setEntityTraitsDatas)
      .def("getStateChangedCallback", &UIDelegateRequest::getStateChangedCallback)
      .def("setStateChangedCallback",
           [](UIDelegateRequest& self, UIDelegateRequest::StateChangedCallback callback) {
             static_cast<PyUIDelegateRequest&>(self).setStateChangedCallback(std::move(callback));
           });

  pyUIDelegateState.def(py::init(&PyUIDelegateState::make))
      .def("__copy__", [](const UIDelegateState& self) { return self.copy(); })
      .def(
          "__deepcopy__",
          [](const UIDelegateState& self, [[maybe_unused]] const py::dict& memo) {
            return self.copy();
          },
          py::arg("memo"))
      .def("copy",
           [](const UIDelegateState& self) {
             return static_cast<const PyUIDelegateState&>(self).copy();
           })
      .def("getNativeData",
           [](const UIDelegateState& self) {
             return static_cast<const PyUIDelegateState&>(self).getPyNativeData();
           })
      .def("setNativeData",
           [](UIDelegateState& self, py::object nativeData) {
             static_cast<PyUIDelegateState&>(self).setPyNativeData(std::move(nativeData));
           })
      .def("getEntityReferences", &UIDelegateState::getEntityReferences)
      .def("setEntityReferences", &UIDelegateState::setEntityReferences)
      .def("getEntityTraitsDatas", &UIDelegateState::getEntityTraitsDatas)
      .def("setEntityTraitsDatas", &UIDelegateState::setEntityTraitsDatas)
      .def("getUpdateRequestCallback", &UIDelegateState::getUpdateRequestCallback)
      .def("setUpdateRequestCallback",
           [](UIDelegateState& self, UIDelegateState::UpdateRequestCallback callback) {
             static_cast<PyUIDelegateState&>(self).setUpdateRequestCallback(std::move(callback));
           });
}
