// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 The Foundry Visionmongers Ltd
#pragma once
#include <memory>

#include <functional>

#include <openassetio/export.h>
#include <openassetio/typedefs.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace managerApi {

OPENASSETIO_DECLARE_PTR(ManagerUIBase)

class OPENASSETIO_CORE_EXPORT ManagerUIBase {
 public:
  OPENASSETIO_ALIAS_PTR(ManagerUIBase)

  virtual void setEntityChosenCallback(std::function<void(EntityReference)> callback) = 0;
  virtual void* getWidget() = 0;

  virtual ~ManagerUIBase();
};
}  // namespace managerApi
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
