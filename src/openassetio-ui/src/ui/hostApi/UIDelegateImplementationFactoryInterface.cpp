// SPDX-License-Identifier: Apache-2.0
// Copyright 2013-2024 The Foundry Visionmongers Ltd
#include <openassetio/ui/hostApi/UIDelegateImplementationFactoryInterface.hpp>

#include <utility>

#include <openassetio/export.h>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace ui::hostApi {

UIDelegateImplementationFactoryInterface::~UIDelegateImplementationFactoryInterface() = default;

UIDelegateImplementationFactoryInterface::UIDelegateImplementationFactoryInterface(
    log::LoggerInterfacePtr logger)
    : logger_{std::move(logger)} {}

const log::LoggerInterfacePtr& UIDelegateImplementationFactoryInterface::logger() const {
  return logger_;
}
}  // namespace ui::hostApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
