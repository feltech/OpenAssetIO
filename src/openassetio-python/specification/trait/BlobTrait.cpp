// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <pybind11/stl.h>

#include <openassetio/specification/trait/BlobTrait.hpp>

#include "../../_openassetio.hpp"

void registerBlobTrait(const py::module& mod) {
  namespace specification = openassetio::specification;
  using openassetio::specification::trait::BlobTrait;

  auto cls = py::class_<BlobTrait, Holder<BlobTrait>>(
      mod, "BlobTrait")
      .def(py::init<Holder<specification::Specification>>())
      .def_static("traitId", &BlobTrait::traitId)
      .def("isValid", &BlobTrait::isValid)
      .def("getUrl", &BlobTrait::getUrl)
      .def("setUrl", &BlobTrait::setUrl)
      .def("getMimeType", &BlobTrait::getMimeType)
      .def("setMimeType", &BlobTrait::setMimeType);
}
