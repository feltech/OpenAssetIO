// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <algorithm>
#include <iterator>
#include <utility>

#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <openassetio/EntityReference.hpp>
#include <openassetio/trait/TraitsData.hpp>
#include <openassetio/trait/collection.hpp>
#include <openassetio/ui/UIDelegateState.hpp>

#include "../_openassetio.hpp"
#include "anyCastToPyObject.hpp"

void registerUIDelegateState(const py::module& mod) {
  using openassetio::EntityReferences;
  using openassetio::trait::TraitsDataPtr;
  using openassetio::trait::TraitsDatas;
  using openassetio::ui::UIDelegateRequest;
  using openassetio::ui::UIDelegateState;

  py::class_<UIDelegateRequest> pyUIDelegateRequest{mod, "UIDelegateRequest"};
  py::class_<UIDelegateState> pyUIDelegateState{mod, "UIDelegateState"};

  pyUIDelegateRequest
      .def(py::init([](py::object nativeData, EntityReferences entityReferences,
                       TraitsDatas entityTraitsDatas, TraitsDataPtr relationshipTraitsData,
                       UIDelegateRequest::StateChangedCallback stateChangedCallback) {
             return UIDelegateRequest{
                 std::move(nativeData), std::move(entityReferences), std::move(entityTraitsDatas),
                 std::move(relationshipTraitsData), std::move(stateChangedCallback)};
           }),
           py::arg("nativeData") = py::none{}, py::arg("entityReferences") = EntityReferences{},
           py::arg("entityTraitsDatas") = TraitsDatas{},
           py::arg("relationshipTraitsData") = py::none{},
           py::arg("stateChangedCallback") = py::none{})
      .def_property(
          "nativeData",
          [](const UIDelegateRequest& self) { return anyCastToPyObject(self.nativeData); },
          [](UIDelegateRequest& self, py::object nativeData) {
            self.nativeData = std::move(nativeData);
          })
      .def_readwrite("entityReferences", &UIDelegateRequest::entityReferences)
      .def_readwrite("entityTraitsDatas", &UIDelegateRequest::entityTraitsDatas)
      .def_readwrite("relationshipTraitsData", &UIDelegateRequest::relationshipTraitsData)
      .def_readwrite("stateChangedCallback", &UIDelegateRequest::stateChangedCallback);

  pyUIDelegateState
      .def(py::init([](py::object nativeData, EntityReferences entityReferences,
                       TraitsDatas entityTraitsDatas,
                       UIDelegateState::UpdateRequestCallback updateRequestCallback) {
             return UIDelegateState{std::move(nativeData), std::move(entityReferences),
                                    std::move(entityTraitsDatas),
                                    std::move(updateRequestCallback)};
           }),
           py::arg("nativeData") = py::none{}, py::arg("entityReferences") = EntityReferences{},
           py::arg("entityTraitsDatas") = TraitsDatas{},
           py::arg("updateRequestCallback") = py::none{})
      .def_property(
          "nativeData",
          [](const UIDelegateState& self) { return anyCastToPyObject(self.nativeData); },
          [](UIDelegateState& self, py::object nativeData) {
            self.nativeData = std::move(nativeData);
          })
      .def_readwrite("entityReferences", &UIDelegateState::entityReferences)
      .def_readwrite("entityTraitsDatas", &UIDelegateState::entityTraitsDatas)
      .def_readwrite("updateRequestCallback", &UIDelegateState::updateRequestCallback);
}