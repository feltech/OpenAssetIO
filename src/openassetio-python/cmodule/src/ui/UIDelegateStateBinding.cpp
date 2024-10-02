// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#include <algorithm>
#include <iterator>
#include <utility>

#include <pybind11/pybind11.h>
#include <pybind11/pytypes.h>
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
  using openassetio::ui::UIDelegateState;

  py::class_<UIDelegateState>(mod, "UIDelegateState")
      .def(py::init())
      .def(py::init([](py::object nativeData, EntityReferences entityReferences,
                       TraitsDatas entityTraitsDatas, TraitsDataPtr relationshipTraitsData) {
             return UIDelegateState{std::move(nativeData), std::move(entityReferences),
                                    std::move(entityTraitsDatas),
                                    std::move(relationshipTraitsData)};
           }),
           py::arg("nativeData"), py::arg("entityReferences"), py::arg("entityTraitsDatas"),
           py::arg("relationshipTraitsData"))
      .def_property(
          "nativeData",
          [](const UIDelegateState& self) { return anyCastToPyObject(self.nativeData); },
          [](UIDelegateState& self, py::object nativeData) {
            self.nativeData = std::move(nativeData);
          })
      .def_readwrite("entityReferences", &UIDelegateState::entityReferences)
      .def_readwrite("entityTraitsDatas", &UIDelegateState::entityTraitsDatas)
      .def_readwrite("relationshipTraitsData", &UIDelegateState::relationshipTraitsData)
      .def("__eq__", [](const UIDelegateState& self, const UIDelegateState& other) {
        // Note: equality is possible in Python but not C++, due to the
        // `std::any` `nativeData` (which in Python is constrained to be
        // a PyObject).
        const py::object& pyNativeData = anyCastToPyObject(self.nativeData);
        const py::object& pyOtherNativeData = anyCastToPyObject(other.nativeData);

        constexpr auto areTraitsDatasEqual =  // NOLINT(readability-identifier-naming)
            [](const TraitsDataPtr& lhs, const TraitsDataPtr& rhs) {
              return lhs.get() == rhs.get() || (lhs && rhs && *lhs == *rhs);
            };

        return pyNativeData.equal(pyOtherNativeData) &&
               self.entityReferences == other.entityReferences &&
               equal(cbegin(self.entityTraitsDatas), cend(self.entityTraitsDatas),
                     cbegin(other.entityTraitsDatas), areTraitsDatasEqual) &&
               areTraitsDatasEqual(self.relationshipTraitsData, other.relationshipTraitsData);
      });
}
