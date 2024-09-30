// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <any>
#include <memory>

#include <pybind11/pybind11.h>

#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/ui/UIDelegateState.hpp>
#include <openassetio/ui/hostApi/UIDelegate.hpp>

namespace py = pybind11;

struct StubUIDelegateRequest : openassetio::ui::UIDelegateRequestInterface {
  StubUIDelegateRequest() = default;
  StubUIDelegateRequest(const StubUIDelegateRequest& other) = delete;
  StubUIDelegateRequest(StubUIDelegateRequest&& other) noexcept = default;
  StubUIDelegateRequest& operator=(const StubUIDelegateRequest& other) = delete;
  StubUIDelegateRequest& operator=(StubUIDelegateRequest&& other) noexcept = default;

  ~StubUIDelegateRequest() override {
    if (nativeData_.has_value()) {
      try {
        auto gil = py::gil_scoped_acquire{};
        (void)py::handle{std::any_cast<PyObject*>(nativeData_)}.dec_ref();
      } catch (...) {  // NOLINT(*-empty-catch)
        // ?
      }
    }
  }
  [[nodiscard]] std::any nativeData() override { return nativeData_; }
  [[nodiscard]] openassetio::EntityReferences entityReferences() override { return {}; }
  [[nodiscard]] openassetio::trait::TraitsDatas entityTraitsDatas() override { return {}; }
  [[nodiscard]] StateChangedCallback stateChangedCallback() override { return {}; }

  std::any nativeData_;
};

extern void registerUIDelegateTestUtils(py::module& mod) {
  using openassetio::ui::UIDelegateRequestInterfacePtr;

  auto ui = mod.def_submodule("ui");

  ui.def("createUIDelegateStateWithNonPyObjectNativeData", []() -> UIDelegateRequestInterfacePtr {
    auto state = std::make_shared<StubUIDelegateRequest>();
    state->nativeData_ = double{};
    return state;
  });
  ui.def("createUIDelegateStateWithRawCPythonNativeData", []() -> UIDelegateRequestInterfacePtr {
    auto state = std::make_shared<StubUIDelegateRequest>();
    state->nativeData_ = PyLong_FromLong(42);  // NOLINT
    return state;
  });
}
