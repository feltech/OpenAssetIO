// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <optional>

#include <pybind11/stl.h>

#include <openassetio/trait/BlobTrait.hpp>

#include "../_openassetio.hpp"

void registerBlobTrait(const py::module& mod) {
  namespace specification = openassetio::specification;
  using openassetio::trait::BlobTrait;
  namespace property = openassetio::trait::property;

  using MaybeStr = std::optional<property::Str>;

  auto cls = py::class_<BlobTrait, Holder<BlobTrait>>(mod, "BlobTrait")
                 .def(py::init<Holder<specification::Specification>>())
                 .def_static("traitId", &BlobTrait::traitId)
                 .def("isValid", &BlobTrait::isValid)
                 .def("getUrl",
                      [](const BlobTrait& self) -> MaybeStr {
                        if (property::Str out; self.getUrl(&out)) {
                          return out;
                        }
                        return {};
                      })
                 .def("setUrl", &BlobTrait::setUrl)
                 .def("getMimeType",
                      [](const BlobTrait& self) -> MaybeStr {
                        if (property::Str out; self.getMimeType(&out)) {
                          return out;
                        }
                        return {};
                      })
                 .def("setMimeType", &BlobTrait::setMimeType);
}
