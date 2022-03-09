// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2022 The Foundry Visionmongers Ltd
#include <openassetio/specification/BlobSpecification.hpp>
#include <openassetio/specification/SpecificationBase.hpp>

#include "../_openassetio.hpp"

void registerBlobSpecification(const py::module& mod) {
  using openassetio::specification::BlobSpecification;
  using openassetio::specification::SpecificationBase;

  py::class_<BlobSpecification, SpecificationBase, Holder<BlobSpecification>>(mod,
                                                                              "BlobSpecification")
      .def(py::init());
}
