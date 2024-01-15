// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#pragma once
#include <cstdint>

#include <openassetio/export.h>
#include <openassetio/typedefs.hpp>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace utils {

enum class PathType : std::uint8_t { kSystem = 0, kPOSIX, kWindows };

OPENASSETIO_CORE_EXPORT Str pathToUrl(StrView absolutePath, PathType pathType = PathType::kSystem);
OPENASSETIO_CORE_EXPORT Str pathFromUrl(StrView fileUrl, PathType pathType = PathType::kSystem);

}  // namespace utils
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
