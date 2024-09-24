// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 The Foundry Visionmongers Ltd
#pragma once
#include <openassetio/export.h>

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
#if defined(_WIN32)
// Shared module library file extension.
constexpr std::string_view kLibExt = ".dll";
// Path separator for encoding multiple search paths in a single string.
constexpr char kPathSep = ';';
#else
// Shared module library file extension.
constexpr std::string_view kLibExt = ".so";
// Path separator for encoding multiple search paths in a single string.
constexpr char kPathSep = ':';
#endif
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
