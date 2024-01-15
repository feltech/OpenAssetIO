// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#include <openassetio/utils/path.hpp>
#include "openassetio/errors/exceptions.hpp"

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace utils {

Str pathToUrl(StrView absolutePath, PathType pathType) {
  (void)absolutePath;
  (void)pathType;
  throw errors::NotImplementedException("pathToUrl not yet implemented");
}

Str pathFromUrl(StrView fileUrl, PathType pathType) {
  (void)fileUrl;
  (void)pathType;
  throw errors::NotImplementedException("pathFromUrl not yet implemented");
}

}  // namespace utils
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
