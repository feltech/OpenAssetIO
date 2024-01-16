// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 The Foundry Visionmongers Ltd
#include <openassetio/utils/path.hpp>

#include <algorithm>

#include <ada.h>
#include <fmt/format.h>

#include <openassetio/errors/exceptions.hpp>

#include "./Regex.hpp"

namespace openassetio {
inline namespace OPENASSETIO_CORE_ABI_VERSION {
namespace utils {

// TODO(DF):
// See https://learn.microsoft.com/en-us/dotnet/standard/io/file-path-formats
// and https://github.com/karwa/swift-url
//  * Refactor to FileURL class - thread-safety + avoiding expensive
//  dangerous statics (i.e. pre-compile regex in constructor).
//    - Regexes are not constants, shouldn't start with `k`
//    - Customise error messages, e.g. "namespaced"->"device path"
//  * Should we support `\\.\` and `\\.\UNC\`?
//    swift-url says:
//    > Windows UNC paths with the hostname "`.`" would be interpreted
//      by the system as referring to a local device (e.g. `\\.\COM1`).
//      These are not files
//    But Windows docs say
//    > the DOS device path syntax, which is one of:
//      \\.\C:\Test\Foo.txt \\?\C:\Test\Foo.txt
//    The confusion in swift-url seems to come from
//    > If the path is a legacy DOS device such as CON, COM1, or LPT1,
//      it is converted into a device path by prepending `\\.\`
//    If we add support, note that
//    > Unless the path starts exactly with \\?\ [...], it is normalized
//    i.e. we need to treat `\\.\` similar to `\\`, but only allow
//    drive paths, not hostnames.
//  * Can/should we support `..` in paths (looks like Ada can handle it
//    for us)?
//    swift-url says:
//    > The URL Standard requires paths in URLs to be fully resolved,
//      and automatically collapsing this path is considered a potential
//      security risk.
//  * Should `/C:` and `/C|` really be %-encoded on posix?
//    swift-url says
//    > These are sometimes interpreted as Windows drive letter
//      delimiters, which POSIX paths obviously do not have.
//  * Should hostnames allow spaces (thus %-encode)?
//    JSON test cases say
//    > Chrome allows this, but it isn't a valid hostname and cannot be
//      escaped. https://github.com/whatwg/url/issues/599
//  * Should we allow `C|`? - Ada seems to convert it to `C:` for us.
//    JSON test cases say
//    > Windows path APIs do not consider these to have drive letters
//    But elsewhere in swift-url test data (for URL parsing) `C|` is
//    normalised to `C:`.

namespace {

struct ForwardSlashSeparatedString {
  Regex kTrailingForwardSlashesInSegmentRegex{R"(//+)"};
  Str replaceTrailingForwardSlashesInPathSegments(const StrView& str) {
    return kTrailingForwardSlashesInSegmentRegex.substituteToReduceSize(str, "/");
  }
};
struct Path {
#ifdef _WIN32
  static constexpr PathType kSystemPathType = PathType::kWindows;
#else
  static constexpr PathType kSystemPathType = PathType::kPOSIX;
#endif
  static constexpr StrView kErrorEmptyPath = "Path is empty";
  static constexpr StrView kErrorRelativePath = "Path is relative";
  static constexpr StrView kErrorUpwardsTraversal = "Path contains upwards traversal";
  static constexpr StrView kErrorNullByte = "Path contains NULL bytes";
  [[nodiscard]] static constexpr PathType resolveSystemPathType(PathType pathType) {
    return pathType == PathType::kSystem ? kSystemPathType : pathType;
  }
  [[nodiscard]] static constexpr bool containsNullByte(const StrView path) {
    return path.find_first_of('\0') != StrView::npos;
  }
};
struct Url {
  static constexpr StrView kErrorNotAFileUrl = "Not a file URL";
  static constexpr StrView kErrorEncodedSeparator = "Percent-encoded path separator";
  Regex kFileUrlRegex{R"(^file://)"};
  [[nodiscard]] bool isFileUrl(const StrView& url) { return kFileUrlRegex.match(url); }
};

// TODO(DF): Customise error messages, e.g. "namespaced"->"device path"
constexpr StrView kErrorNotAFileUrl = "Not a file URL";
constexpr StrView kErrorEmptyPath = "Path is empty";
constexpr StrView kErrorInvalidPath = "Path is ill-formed";
constexpr StrView kErrorRelativePath = "Path is relative";
constexpr StrView kErrorUpwardsTraversal = "Path contains upwards traversal";
constexpr StrView kErrorNullByte = "Path contains NULL bytes";
constexpr StrView kErrorUnsupportedHostname = "Unsupported hostname";
constexpr StrView kErrorEncodedSeparator = "Percent-encoded path separator";

#ifdef _WIN32
constexpr PathType kSystemPathType = PathType::kWindows;
#else
constexpr PathType kSystemPathType = PathType::kPOSIX;
#endif
constexpr char kColon = ':';
constexpr char kPercent = '%';
constexpr char kHyphen = '-';
constexpr StrView kAnySlash = "\\/";
constexpr char kForwardSlash = '/';
constexpr char kBackSlash = '\\';

Regex kFileUrlRegex{R"(^file://)"};
Regex kLocalHostRegex{"^localhost$"};
Regex kPercentEncodedForwardSlashRegex{R"(%2F)"};
Regex kPercentEncodedSlashRegex{R"(%(:?5C|2F))"};
Regex kTrailingForwardSlashesInSegmentRegex{R"(//+)"};

[[nodiscard]] bool isFileUrl(const StrView& url) { return kFileUrlRegex.match(url); }

[[nodiscard]] constexpr PathType resolveSystemPathType(PathType pathType) {
  return pathType == PathType::kSystem ? kSystemPathType : pathType;
}

void throwError(const StrView message, const StrView pathOrUrl) {
  throw errors::InputValidationException(fmt::format("{} ('{}')", message, pathOrUrl));
}

[[nodiscard]] bool containsNullByte(const StrView path) {
  return path.find_first_of('\0') != StrView::npos;
}

[[nodiscard]] bool containsPercentEncodedForwardSlash(const StrView& str) {
  // Using regex for case-insensitivity.
  return kPercentEncodedForwardSlashRegex.match(str);
}

[[nodiscard]] bool containsPercentEncodedSlash(const StrView& str) {
  // Using regex for case-insensitivity.
  return kPercentEncodedSlashRegex.match(str);
}

Str replaceTrailingForwardSlashesInPathSegments(const StrView& str) {
  return kTrailingForwardSlashesInSegmentRegex.substituteToReduceSize(str, "/");
}

namespace win {

struct UncDetails {
  bool isDevicePath;     // i.e. if starts with `\\?\`
  bool isUncDevicePath;  // i.e. if starts with `\\?\UNC\`
  bool isDrivePath;      // i.e. if server part is drive letter e.g. \\?\C:
  StrView hostOrDrive;
  StrView shareName;
  StrView sharePath;
  StrView shareNameAndPath;
  StrView fullPath;
};
struct Path {
  static constexpr StrView kErrorInvalidPath = "Path is ill-formed";
};

struct NormalisedPath {
  Regex kUpwardsTraversalRegex{R"((^|[\\/])\.\.([\\/]|$))"};
  Regex kTrailingDotsAsFileRegex{R"([\\/](\.{3,})$)"};
  Regex kTrailingDotsInFileRegex{R"([^.\\/](\.+)$)"};
  Regex kTrailingDotsAndSpacesRegex{R"([\\/][^\\/ ]*( [. ]*)$)"};
  Regex kTrailingSlashesRegex{R"([\\/]([\\/]+)$)"};
  Regex kTrailingSingleDotInSegmentRegex{R"((?<![.\\/])\.(?=[/\\]))"};
  Regex kTrailingSlashesInSegmentRegex{R"(([\\/])[\\/]+)"};

  [[nodiscard]] StrView withoutTrailingSlashes(const StrView& path) {
    if (!kTrailingSlashesRegex.match(path)) {
      return path;
    }
    return path.substr(0, path.size() - kTrailingSlashesRegex.lastMatchGroup(1).size());
  }
  [[nodiscard]] StrView withoutTrailingDotsAsFile(const StrView& path) {
    if (!kTrailingDotsAsFileRegex.match(path)) {
      return path;
    }
    return path.substr(0, path.size() - kTrailingDotsAsFileRegex.lastMatchGroup(1).size());
  }
  [[nodiscard]] StrView withoutTrailingDotsInFile(const StrView& path) {
    if (!kTrailingDotsInFileRegex.match(path)) {
      return path;
    }
    return path.substr(0, path.size() - kTrailingDotsInFileRegex.lastMatchGroup(1).size());
  }
  [[nodiscard]] StrView withoutTrailingDotsOrSpaces(const StrView& path) {
    if (!kTrailingDotsAndSpacesRegex.match(path)) {
      return path;
    }
    return path.substr(0, path.size() - kTrailingDotsAndSpacesRegex.lastMatchGroup(1).size());
  }
  [[nodiscard]] bool containsUpwardsTraversal(const StrView str) {
    return kUpwardsTraversalRegex.match(str);
  }
};
struct DrivePath {
  Regex kDriveRegex{R"(^[A-Z][\|:])"};
  Regex kAbsoluteDrivePathRegex{R"(^[A-Z]:[/\\])"};

  bool isDrive(const StrView& str) { return kDriveRegex.match(str); }
  [[nodiscard]] bool isAbsoluteDrivePath(const StrView& str) {
    return kAbsoluteDrivePathRegex.match(str);
  }
};
struct UncPath {
  static constexpr StrView kDoubleBackSlash = R"(\\)";
};
struct UncHost {
  /**
   * Invalid UNC hostname regex.
   *
   * - Unicode domains are unsupported, so ensure ASCII.
   * - Ensure no %-encoding.
   * - Reject "?" and "." as UNC hostnames. From swift-url code comments:
   *   > Otherwise we might create something which looks like a Win32 file
   *     namespace/local device path
   */
  Regex kInvalidHostnameRegex{R"(^[.?]$|[^[:ascii:]]|%)"};

  [[nodiscard]] bool isInvalidHostname(const StrView& str) {
    return kInvalidHostnameRegex.match(str);
  }
};
struct UncSharePath {
  DrivePath& drivePathHandler;
  NormalisedPath& normalisedPathHandler;

  static constexpr StrView kErrorInvalidHostname = "Path references an invalid hostname";
  static constexpr StrView kLocalHostIP = "127.0.0.1";
  static constexpr StrView kIp6HostSuffix = ".ipv6-literal.net";
  Regex kUncPathRegex{R"(^([\\/]{2,})([^\\/]*)(.*)$)"};
  Regex kPathHeadAndTailRegex{R"(^([\\/]+[^\\/]+)([\\/].*)$)"};

  [[nodiscard]] std::optional<UncDetails> extractUncDetails(const StrView path) {
    if (!kUncPathRegex.match(path)) {
      return std::nullopt;
    }
    const bool isDevicePath = false;
    const bool isUncDevicePath = false;
    const StrView prefix = kUncPathRegex.lastMatchGroup(1);
    const StrView hostOrDrive = kUncPathRegex.lastMatchGroup(2);
    const bool isDrivePath = drivePathHandler.isDrive(path);
    const auto [shareName, sharePath, shareNameAndPath] =
        extractShareNameAndPath(kUncPathRegex.lastMatchGroup(3));
    const StrView fullPath =
        path.substr(prefix.size(), hostOrDrive.size() + shareNameAndPath.size());
    return UncDetails{isDevicePath, isUncDevicePath, isDrivePath,      hostOrDrive,
                      shareName,    sharePath,       shareNameAndPath, fullPath};
  }

  std::tuple<StrView, StrView, StrView> extractShareNameAndPath(StrView shareNameAndPath) {
    shareNameAndPath = normalisedPathHandler.withoutTrailingSlashes(shareNameAndPath);
    if (!kPathHeadAndTailRegex.match(shareNameAndPath)) {
      return {{}, shareNameAndPath, shareNameAndPath};
    }
    const StrView shareName = kPathHeadAndTailRegex.lastMatchGroup(1);
    const StrView sharePath = normalisedPathHandler.withoutTrailingDotsInFile(
        normalisedPathHandler.withoutTrailingDotsAsFile(
            normalisedPathHandler.withoutTrailingDotsOrSpaces(
                kPathHeadAndTailRegex.lastMatchGroup(2))));
    // In case shareNameAndPath is now shorter due to trimming trailing
    // dots/spaces.
    shareNameAndPath = shareNameAndPath.substr(0, shareName.size() + sharePath.size());
    return {shareName, sharePath, shareNameAndPath};
  }
};
struct UncUnnormalisedDevicePath {
  // E.g. Device path with forward slashes - technically allowed (as a
  // literal rather than path separator) but unsupported by us.
  static constexpr StrView kErrorUnsupportedDevicePath = "Unsupported Win32 namespaced path";
  Regex kDeviceUpwardsTraversalRegex{R"((^|\\)\.\.(\\|$))"};
  Regex kDeviceTrailingSlashesRegex{R"(\\(\\+)$)"};
  Regex kDeviceTrailingSlashesInSegmentRegex{R"((\\\\+))"};

  [[nodiscard]] StrView withoutDeviceTrailingSlashes(const StrView& path) {
    if (!kDeviceTrailingSlashesRegex.match(path)) {
      return path;
    }
    return path.substr(0, path.size() - kDeviceTrailingSlashesRegex.lastMatchGroup(1).size());
  }
  [[nodiscard]] constexpr static bool containsForwardSlash(const StrView path) {
    return path.find_first_of(kForwardSlash) != StrView::npos;
  }
  [[nodiscard]] bool containsDeviceUpwardsTraversal(const StrView& str) {
    return kDeviceUpwardsTraversalRegex.match(str);
  }
};
struct UncUnnormalisedDeviceDrivePath {
  DrivePath& drivePathHandler;
  UncUnnormalisedDevicePath& uncUnnormalisedDevicePathHandler;

  static constexpr std::size_t kDevicePathPrefixLength = StrView{R"(\\?\)"}.size();
  Regex kDevicePathRegex{R"(^\\\\\?\\([^\\]*)(.*)$)"};

  [[nodiscard]] std::optional<UncDetails> extractUncDetails(const StrView path) {
    if (!kDevicePathRegex.match(path)) {
      return std::nullopt;
    }
    const bool isDevicePath = true;
    const bool isUncDevicePath = false;
    const StrView hostOrDrive = kDevicePathRegex.lastMatchGroup(1);
    const bool isDrivePath = drivePathHandler.isDrive(hostOrDrive);
    const StrView shareNameAndPath = uncUnnormalisedDevicePathHandler.withoutDeviceTrailingSlashes(
        kDevicePathRegex.lastMatchGroup(2));
    const StrView fullPath =
        path.substr(kDevicePathPrefixLength, hostOrDrive.size() + shareNameAndPath.size());
    return UncDetails{isDevicePath, isUncDevicePath,  isDrivePath, hostOrDrive, {},
                      {},           shareNameAndPath, fullPath};
  }
};
struct UncUnnormalisedDeviceSharePath {
  DrivePath& drivePathHandler;
  UncUnnormalisedDevicePath& uncUnnormalisedDevicePathHandler;

  static constexpr std::size_t kUncDevicePathPrefixLength = StrView{R"(\\?\UNC\)"}.size();
  Regex kUncDevicePathRegex{R"(^\\\\\?\\UNC\\([^\\]*)(.*)$)"};
  Regex kDevicePathHeadAndTailRegex{R"(^(\\[^\\]+)(.+)$)"};

  [[nodiscard]] std::optional<UncDetails> extractUncDetails(const StrView path) {
    if (!kUncDevicePathRegex.match(path)) {
      return std::nullopt;
    }
    const bool isDevicePath = true;
    const bool isUncDevicePath = true;
    const StrView hostOrDrive = kUncDevicePathRegex.lastMatchGroup(1);
    const bool isDrivePath = drivePathHandler.isDrive(hostOrDrive);
    const auto [shareName, sharePath, shareNameAndPath] =
        extractShareNameAndPath(kUncDevicePathRegex.lastMatchGroup(2));
    const StrView fullPath =
        path.substr(kUncDevicePathPrefixLength, hostOrDrive.size() + shareNameAndPath.size());
    return UncDetails{isDevicePath, isUncDevicePath, isDrivePath,      hostOrDrive,
                      shareName,    sharePath,       shareNameAndPath, fullPath};
  }

  std::tuple<StrView, StrView, StrView> extractShareNameAndPath(StrView shareNameAndPath) {
    shareNameAndPath =
        uncUnnormalisedDevicePathHandler.withoutDeviceTrailingSlashes(shareNameAndPath);
    if (!kDevicePathHeadAndTailRegex.match(shareNameAndPath)) {
      return {{}, shareNameAndPath, shareNameAndPath};
    }
    return {kDevicePathHeadAndTailRegex.lastMatchGroup(1),
            kDevicePathHeadAndTailRegex.lastMatchGroup(2), shareNameAndPath};
  }
};

struct PathToUrl {
  DrivePath drivePathHandler{};
  UncUnnormalisedDevicePath uncUnnormalisedDevicePathHandler{};
  UncUnnormalisedDeviceSharePath uncUnnormalisedDeviceSharePathHandler{
      drivePathHandler, uncUnnormalisedDevicePathHandler};
  UncUnnormalisedDeviceDrivePath uncUnnormalisedDeviceDrivePathHandler{
      drivePathHandler, uncUnnormalisedDevicePathHandler};
  NormalisedPath normalisedPathHandler{};
  UncSharePath uncSharePathHandler{drivePathHandler, normalisedPathHandler};

  [[nodiscard]] std::optional<UncDetails> extractUncDetails(const StrView path) {
    if (auto uncDetails = uncUnnormalisedDeviceSharePathHandler.extractUncDetails(path)) {
      return uncDetails;
    }
    if (auto uncDetails = uncUnnormalisedDeviceDrivePathHandler.extractUncDetails(path)) {
      return uncDetails;
    }
    if (auto uncDetails = uncSharePathHandler.extractUncDetails(path)) {
      return uncDetails;
    }
    return std::nullopt;
  }
};

struct Url {
  static constexpr StrView kErrorUnsupportedHostname = "Unsupported hostname";
  static constexpr StrView kDoubleBackSlash = R"(\\)";
  static constexpr StrView kLocalHostIP = "127.0.0.1";
  static constexpr StrView kIp6HostSuffix = ".ipv6-literal.net";
  Regex kIp6HostRegex{R"(^\[([A-Z0-9:]+)\]$)"};
  Regex kLocalHostRegex{"^localhost$"};
  Regex kPercentEncodedSlashRegex{R"(%(:?5C|2F))"};
  /**
   * Augment default percent encoded set for paths.
   *
   * From swift-url's` `WindowsPathEncodeSet` docstring:
   *
   * - The '%' sign itself. Filesystem paths do not contain
   * percent-encoding, and any character sequences which look like
   * percent-encoding are just coincidences.
   * - Note that the colon character (`:`) is also included, so this
   * encode-set is not appropriate for Windows drive letter components.
   * Drive letters should not be percent-encoded.
   */
  static constexpr std::array kPercentEncodeCharacterSet = [] {
    constexpr std::uint8_t kByteSize = 8;
    constexpr std::size_t kArrSize = 32;  // 0xFF/kByteSize + 1;

    constexpr std::uint8_t kPercentHex = 0x25;
    constexpr std::uint8_t kColonHex = 0x3A;
    constexpr std::uint8_t kVerticalBarHex = 0x7C;

    std::array<uint8_t, kArrSize> charSet{};
    // Copy Ada's default %-encode set for URL path components.
    for (std::size_t idx = 0; idx < charSet.size(); ++idx) {
      charSet[idx] = ada::character_sets::PATH_PERCENT_ENCODE[idx];
    }
    // Augment the %-encode set with additional characters.
    for (std::uint8_t charCode : {kPercentHex, kColonHex, kVerticalBarHex}) {
      charSet[charCode / kByteSize] |= static_cast<std::uint8_t>(1 << (charCode % kByteSize));
    }

    return charSet;
  }();

  [[nodiscard]] bool containsPercentEncodedSlash(const StrView& str) {
    // Using regex for case-insensitivity.
    return kPercentEncodedSlashRegex.match(str);
  }
  [[nodiscard]] bool isLocalHost(const StrView& str) { return kLocalHostRegex.match(str); }
};
}  // namespace win
namespace posix {
struct Path {
  Regex kUpwardsTraversalRegex{R"((^|/)\.\.(/|$))"};

  [[nodiscard]] bool containsUpwardsTraversal(const StrView& str) {
    return kUpwardsTraversalRegex.match(str);
  }

  [[nodiscard]] static bool startsWithForwardSlash(const StrView str) {
    // Precondition
    assert(!str.empty());
    return str.front() == '/';
  }
  [[nodiscard]] static Str collapseForwardSlashes(const StrView& path) {
    if (path.size() <= 2) {
      return Str{path};
    }
    Str normalisedPath;
    normalisedPath.reserve(path.size());
    if (path[0] == kForwardSlash && path[1] == kForwardSlash && path[2] != kForwardSlash) {
      // Apparently two leading `/`s are implementation defined, so should
      // be retained. Any more than two should be collapsed to one.
      const StrView pathView{path};
      normalisedPath += pathView.substr(0, 2);
      normalisedPath += replaceTrailingForwardSlashesInPathSegments(pathView.substr(2));
    } else {
      normalisedPath = replaceTrailingForwardSlashesInPathSegments(path);
    }
    return normalisedPath;
  }
};
struct Url {
  Regex kPercentEncodedForwardSlashRegex{R"(%2F)"};
  [[nodiscard]] bool containsPercentEncodedForwardSlash(const StrView& str) {
    // Using regex for case-insensitivity.
    return kPercentEncodedForwardSlashRegex.match(str);
  }
};
struct PathToUrl {
  Path pathHandler{};
  ForwardSlashSeparatedString forwardSlashSeparatedStringHandler{};

  [[nodiscard]] Str pathToUrl(const StrView linuxPath) {
    // Precondition.
    assert(!linuxPath.empty());

    if (pathHandler.containsUpwardsTraversal(linuxPath)) {
      throwError(kErrorUpwardsTraversal, linuxPath);
    }
    if (!Path::startsWithForwardSlash(linuxPath)) {
      throwError(kErrorRelativePath, linuxPath);
    }

    ada::url adaUrl;
    adaUrl.type = ada::scheme::FILE;
    // Must explicitly set empty host to get `file://` rather than
    // `file:`.
    adaUrl.set_host("");

    Str processedPath;
    // Ada will %-encode, but with a more limited set that we want.
    ada::unicode::percent_encode<false>(linuxPath, kPercentEncodeCharacterSet.data(),
                                        processedPath);

    if (processedPath.empty()) {
      processedPath = linuxPath;
    }
    // Collapse multiple `/` to single, except at the beginning, where
    // `//` is valid.
    processedPath = Path::collapseForwardSlashes(processedPath);

    if (!adaUrl.set_pathname(processedPath)) {
      throwError("Failed to create URL path from UNC path", linuxPath);
    }

    return adaUrl.get_href();
  }
};
}  // namespace posix
namespace win {

constexpr StrView kErrorInvalidHostname = "Path references an invalid hostname";
// E.g. Device path with forward slashes - technically allowed (as a
// literal rather than path separator) but unsupported by us.
constexpr StrView kErrorUnsupportedDevicePath = "Unsupported Win32 namespaced path";

constexpr StrView kDoubleBackSlash = R"(\\)";
constexpr StrView kLocalHostIP = "127.0.0.1";
constexpr StrView kIp6HostSuffix = ".ipv6-literal.net";

// Apparently `C|` is a valid drive letter in some cases.
Regex kDriveRegex{R"(^[A-Z][\|:])"};
Regex kAbsoluteDrivePathRegex{R"(^[A-Z]:[/\\])"};

Regex kUpwardsTraversalRegex{R"((^|[\\/])\.\.([\\/]|$))"};
Regex kDeviceUpwardsTraversalRegex{R"((^|\\)\.\.(\\|$))"};

constexpr std::size_t kDriveLetterLength = 2;
constexpr std::size_t kDevicePathPrefixLength = StrView{R"(\\?\)"}.size();
constexpr std::size_t kUncDevicePathPrefixLength = StrView{R"(\\?\UNC\)"}.size();

Regex kUncPathRegex{R"(^([\\/]{2,})([^\\/]*)(.*)$)"};
Regex kDevicePathRegex{R"(^\\\\\?\\([^\\]*)(.*)$)"};
Regex kUncDevicePathRegex{R"(^\\\\\?\\UNC\\([^\\]*)(.*)$)"};

/**
 * Invalid UNC hostname regex.
 *
 * - Unicode domains are unsupported, so ensure ASCII.
 * - Ensure no %-encoding.
 * - Reject "?" and "." as UNC hostnames. From swift-url code comments:
 *   > Otherwise we might create something which looks like a Win32 file
 *     namespace/local device path
 */
Regex kInvalidHostnameRegex{R"(^[.?]$|[^[:ascii:]]|%)"};

Regex kPathHeadAndTailRegex{R"(^([\\/]+[^\\/]+)([\\/].*)$)"};
Regex kDevicePathHeadAndTailRegex{R"(^(\\[^\\]+)(.+)$)"};
Regex kTrailingDotsAsFileRegex{R"([\\/](\.{3,})$)"};
Regex kTrailingDotsInFileRegex{R"([^.\\/](\.+)$)"};
Regex kTrailingDotsAndSpacesRegex{R"([\\/][^\\/ ]*( [. ]*)$)"};
Regex kTrailingSlashesRegex{R"([\\/]([\\/]+)$)"};
Regex kDeviceTrailingSlashesRegex{R"(\\(\\+)$)"};

Regex kTrailingSingleDotInSegmentRegex{R"((?<![.\\/])\.(?=[/\\]))"};
Regex kTrailingSlashesInSegmentRegex{R"(([\\/])[\\/]+)"};
Regex kDeviceTrailingSlashesInSegmentRegex{R"((\\\\+))"};

Regex kIp6HostRegex{R"(^\[([A-Z0-9:]+)\]$)"};

/**
 * Augment default percent encoded set for paths.
 *
 * From swift-url's` `WindowsPathEncodeSet` docstring:
 *
 * - The '%' sign itself. Filesystem paths do not contain
 * percent-encoding, and any character sequences which look like
 * percent-encoding are just coincidences.
 * - Note that the colon character (`:`) is also included, so this
 * encode-set is not appropriate for Windows drive letter components.
 * Drive letters should not be percent-encoded.
 */
constexpr std::array kPercentEncodeCharacterSet = [] {
  constexpr std::uint8_t kByteSize = 8;
  constexpr std::size_t kArrSize = 32;  // 0xFF/kByteSize + 1;

  constexpr std::uint8_t kPercentHex = 0x25;
  constexpr std::uint8_t kColonHex = 0x3A;
  constexpr std::uint8_t kVerticalBarHex = 0x7C;

  std::array<uint8_t, kArrSize> charSet{};
  // Copy Ada's default %-encode set for URL path components.
  for (std::size_t idx = 0; idx < charSet.size(); ++idx) {
    charSet[idx] = ada::character_sets::PATH_PERCENT_ENCODE[idx];
  }
  // Augment the %-encode set with additional characters.
  for (std::uint8_t charCode : {kPercentHex, kColonHex, kVerticalBarHex}) {
    charSet[charCode / kByteSize] |= static_cast<std::uint8_t>(1 << (charCode % kByteSize));
  }

  return charSet;
}();

[[nodiscard]] StrView withoutTrailingSlashes(const StrView& path) {
  if (!kTrailingSlashesRegex.match(path)) {
    return path;
  }
  return path.substr(0, path.size() - kTrailingSlashesRegex.lastMatchGroup(1).size());
}

[[nodiscard]] StrView withoutDeviceTrailingSlashes(const StrView& path) {
  if (!kDeviceTrailingSlashesRegex.match(path)) {
    return path;
  }
  return path.substr(0, path.size() - kDeviceTrailingSlashesRegex.lastMatchGroup(1).size());
}

[[nodiscard]] StrView withoutTrailingDotsAsFile(const StrView& path) {
  if (!kTrailingDotsAsFileRegex.match(path)) {
    return path;
  }
  return path.substr(0, path.size() - kTrailingDotsAsFileRegex.lastMatchGroup(1).size());
}

[[nodiscard]] StrView withoutTrailingDotsInFile(const StrView& path) {
  if (!kTrailingDotsInFileRegex.match(path)) {
    return path;
  }
  return path.substr(0, path.size() - kTrailingDotsInFileRegex.lastMatchGroup(1).size());
}

[[nodiscard]] StrView withoutTrailingDotsOrSpaces(const StrView& path) {
  if (!kTrailingDotsAndSpacesRegex.match(path)) {
    return path;
  }
  return path.substr(0, path.size() - kTrailingDotsAndSpacesRegex.lastMatchGroup(1).size());
}
[[nodiscard]] std::optional<UncDetails> extractUncDetails(const StrView path) {
  if (kUncDevicePathRegex.match(path)) {
    const bool isDevicePath = true;
    const bool isUncDevicePath = true;
    const StrView hostOrDrive = kUncDevicePathRegex.lastMatchGroup(1);
    const bool isDrivePath = kDriveRegex.match(hostOrDrive);
    const StrView shareNameAndPath =
        withoutDeviceTrailingSlashes(kUncDevicePathRegex.lastMatchGroup(2));
    StrView shareName{shareNameAndPath};
    StrView sharePath;
    if (kDevicePathHeadAndTailRegex.match(shareNameAndPath)) {
      shareName = kDevicePathHeadAndTailRegex.lastMatchGroup(1);
      sharePath = kDevicePathHeadAndTailRegex.lastMatchGroup(2);
    }
    const StrView fullPath =
        path.substr(kUncDevicePathPrefixLength, hostOrDrive.size() + shareNameAndPath.size());
    return UncDetails{isDevicePath, isUncDevicePath, isDrivePath,      hostOrDrive,
                      shareName,    sharePath,       shareNameAndPath, fullPath};
  }
  if (kDevicePathRegex.match(path)) {
    const bool isDevicePath = true;
    const bool isUncDevicePath = false;
    const StrView hostOrDrive = kDevicePathRegex.lastMatchGroup(1);
    const bool isDrivePath = kDriveRegex.match(hostOrDrive);
    const StrView shareNameAndPath =
        withoutDeviceTrailingSlashes(kDevicePathRegex.lastMatchGroup(2));
    const StrView fullPath =
        path.substr(kDevicePathPrefixLength, hostOrDrive.size() + shareNameAndPath.size());
    return UncDetails{isDevicePath, isUncDevicePath,  isDrivePath, hostOrDrive, {},
                      {},           shareNameAndPath, fullPath};
  }
  if (kUncPathRegex.match(path)) {
    const bool isDevicePath = false;
    const bool isUncDevicePath = false;
    const StrView prefix = kUncPathRegex.lastMatchGroup(1);
    const StrView hostOrDrive = kUncPathRegex.lastMatchGroup(2);
    const bool isDrivePath = kDriveRegex.match(hostOrDrive);
    StrView shareNameAndPath = withoutTrailingSlashes(kUncPathRegex.lastMatchGroup(3));
    StrView shareName{shareNameAndPath};
    StrView sharePath;
    if (kPathHeadAndTailRegex.match(shareNameAndPath)) {
      shareName = kPathHeadAndTailRegex.lastMatchGroup(1);
      sharePath = withoutTrailingDotsInFile(withoutTrailingDotsAsFile(
          withoutTrailingDotsOrSpaces(kPathHeadAndTailRegex.lastMatchGroup(2))));
      // In case shareNameAndPath is now shorter due to trimming trailing
      // dots/spaces.
      shareNameAndPath = shareNameAndPath.substr(0, shareName.size() + sharePath.size());
    }
    const StrView fullPath =
        path.substr(prefix.size(), hostOrDrive.size() + shareNameAndPath.size());
    return UncDetails{isDevicePath, isUncDevicePath, isDrivePath,      hostOrDrive,
                      shareName,    sharePath,       shareNameAndPath, fullPath};
  }

  return std::nullopt;
}

[[nodiscard]] bool containsForwardSlash(const StrView path) {
  return path.find_first_of(kForwardSlash) != StrView::npos;
}

[[nodiscard]] bool isLocalHost(const StrView& str) { return kLocalHostRegex.match(str); }

[[nodiscard]] bool isInvalidHostname(const StrView& str) {
  return kInvalidHostnameRegex.match(str);
}

[[nodiscard]] bool isAbsoluteDrivePath(const StrView& str) {
  return kAbsoluteDrivePathRegex.match(str);
}

[[nodiscard]] bool containsUpwardsTraversal(const StrView& str) {
  return kUpwardsTraversalRegex.match(str);
}

[[nodiscard]] bool containsDeviceUpwardsTraversal(const StrView& str) {
  return kDeviceUpwardsTraversalRegex.match(str);
}

bool startsWithSlash(const StrView& windowsPath) {
  // Precondition.
  assert(!windowsPath.empty());
  return windowsPath.substr(0, 1).find_first_of(kAnySlash) != StrView::npos;
}

Str replaceTrailingDotsInPathSegments(const StrView& path) {
  return kTrailingSingleDotInSegmentRegex.substituteToReduceSize(path, "");
}

Str replaceTrailingSlashesInPathSegments(const StrView& path) {
  return kTrailingSlashesInSegmentRegex.substituteToReduceSize(path, "$1");
}

Str replaceDeviceTrailingSlashesInPathSegments(const StrView& path) {
  return kDeviceTrailingSlashesInSegmentRegex.substituteToReduceSize(path, R"(\)");
}

void validateDevicePath(const StrView& windowsPath, const UncDetails& uncDetails) {
  if (uncDetails.fullPath.empty()) {
    // Must have something after the `\\?\` or `\\?\UNC\`
    throwError(kErrorInvalidPath, windowsPath);
  }
  if (containsForwardSlash(uncDetails.fullPath)) {
    // Don't support verbatim `/` in UNC device paths, for now.
    throwError(kErrorUnsupportedDevicePath, windowsPath);
  }
  if (containsDeviceUpwardsTraversal(uncDetails.shareNameAndPath)) {
    // Disallow `..`, except for hostnames.
    throwError(kErrorUpwardsTraversal, windowsPath);
  }
  if (uncDetails.isUncDevicePath) {
    if (uncDetails.hostOrDrive.empty()) {
      // E.g. `\\?\UNC\\path` - host segment is blank.
      throwError(kErrorInvalidHostname, windowsPath);
    }
    // `\\?\\UNC\`
    if (uncDetails.isDrivePath) {
      // Must be a hostname not drive letter.
      throwError(kErrorInvalidHostname, windowsPath);
    }
    if (isInvalidHostname(uncDetails.hostOrDrive)) {
      // Host is empty or contains disallowed characters.
      throwError(kErrorInvalidHostname, windowsPath);
    }
  } else {
    if (uncDetails.hostOrDrive.empty()) {
      // E.g. `\\?\\path` - drive letter segment is blank.
      throwError(kErrorInvalidPath, windowsPath);
    }
    // `\\?\C:\path`
    if (uncDetails.shareNameAndPath.empty()) {
      // Must be followed by an absolute path e.g. `\\?\C:\`.
      throwError(kErrorInvalidPath, windowsPath);
    }
    if (!uncDetails.isDrivePath) {
      // Must be followed by a drive e.g. `\\?\C:`.
      throwError(kErrorUnsupportedDevicePath, windowsPath);
    }
    if (uncDetails.hostOrDrive.back() != kColon) {
      // Disallow drive letters ending in `|`.
      throwError(kErrorUnsupportedDevicePath, windowsPath);
    }
  }
}

void validateUncPath(const StrView& windowsPath, const UncDetails& uncDetails) {
  if (uncDetails.fullPath.empty()) {
    throwError(kErrorInvalidPath, windowsPath);
  }

  if (uncDetails.isDevicePath) {
    validateDevicePath(windowsPath, uncDetails);
  } else {
    if (containsUpwardsTraversal(uncDetails.shareNameAndPath)) {
      // Disallow `..`, except for hostnames.
      throwError(kErrorUpwardsTraversal, windowsPath);
    }
    if (uncDetails.isDrivePath) {
      // Disallow drive letters as UNC host e.g. `\\C:\`
      throwError(kErrorInvalidHostname, windowsPath);
    }
    if (isInvalidHostname(uncDetails.hostOrDrive)) {
      // Host is empty or contains disallowed characters.
      throwError(kErrorInvalidHostname, windowsPath);
    }
  }
}

void validateDrivePath(const StrView& windowsPath) {
  // TODO(DF): Kludge to match error priority of swift-url. Otherwise
  // this would be handled by `isAbsoluteDrivePath`.
  if (startsWithSlash(windowsPath)) {
    throwError(kErrorRelativePath, windowsPath);
  }
  if (containsUpwardsTraversal(windowsPath)) {
    throwError(kErrorUpwardsTraversal, windowsPath);
  }
  if (!isAbsoluteDrivePath(windowsPath)) {
    throwError(kErrorRelativePath, windowsPath);
  }
}

bool maybePercentEncodeAndAppendTo(const StrView& path, Str& appendTo) {
  // Ada will %-encode but with a more limited set that we want (e.g. we want to encode `%`).
  return ada::unicode::percent_encode<true>(path, kPercentEncodeCharacterSet.data(), appendTo);
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void setUrlPathFromDrivePath(const StrView& windowsPath, ada::url& url) {
  // Precondition.
  assert(isAbsoluteDrivePath(windowsPath));

  const auto setPath = [&url, &windowsPath](const StrView& path) {
    if (!url.set_pathname(path)) {
      throwError("Failed to create URL path from windows path", windowsPath);
    }
  };

  const StrView trimmedPath = withoutTrailingDotsInFile(
      withoutTrailingDotsAsFile(withoutTrailingDotsOrSpaces(withoutTrailingSlashes(windowsPath))));
  const Str normalisedPath =
      replaceTrailingDotsInPathSegments(replaceTrailingSlashesInPathSegments(trimmedPath));

  const StrView normalisedPathView{normalisedPath};
  const StrView driveLetter = normalisedPathView.substr(0, kDriveLetterLength);
  const StrView drivePath = normalisedPathView.substr(kDriveLetterLength);

  if (Str encodedPath{driveLetter}; maybePercentEncodeAndAppendTo(drivePath, encodedPath)) {
    setPath(encodedPath);
  } else {
    setPath(normalisedPath);
  }
}

void setUrlHostFromUncPath(const StrView& originalPath, const UncDetails& uncDetails,
                           ada::url& url) {
  const auto setHost = [&url, &originalPath](const StrView& host) {
    if (!url.set_hostname(host)) {
      throwError(kErrorInvalidHostname, originalPath);
    }
  };

  if (!uncDetails.isDrivePath) {
    if (isLocalHost(uncDetails.hostOrDrive)) {
      setHost(kLocalHostIP);
    } else {
      setHost(uncDetails.hostOrDrive);
    }
  } else {
    url.set_host("");
  }
}

void setUrlPathFromUncPath(const StrView& originalPath, const UncDetails& uncDetails,
                           ada::url& url) {
  const auto setPath = [&originalPath, &url](const StrView& path) {
    if (!url.set_pathname(path)) {
      throwError("Failed to set URL path from UNC path", originalPath);
    }
  };
  const auto encodeAndSetPath = [&setPath](const auto& path) {
    // Percent encode what's left.
    if (Str encodedPath; maybePercentEncodeAndAppendTo(path, encodedPath)) {
      setPath(encodedPath);
    } else {
      setPath(path);
    }
  };
  if (uncDetails.isUncDevicePath) {
    // `\\?\UNC\host\share\path`
    encodeAndSetPath(replaceDeviceTrailingSlashesInPathSegments(uncDetails.shareNameAndPath));
  } else if (uncDetails.isDevicePath) {
    // `\\?\C:\path` - `C:` part should not be %-encoded.
    if (Str encodedPath{uncDetails.hostOrDrive}; maybePercentEncodeAndAppendTo(
            replaceDeviceTrailingSlashesInPathSegments(uncDetails.shareNameAndPath),
            encodedPath)) {
      setPath(replaceDeviceTrailingSlashesInPathSegments(encodedPath));
    } else {
      setPath(replaceDeviceTrailingSlashesInPathSegments(uncDetails.fullPath));
    }
  } else {
    // `\\host\share\path`
    Str normalisedPath{uncDetails.shareName};
    normalisedPath += replaceTrailingDotsInPathSegments(uncDetails.sharePath);
    encodeAndSetPath(replaceTrailingSlashesInPathSegments(normalisedPath));
  }
}
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void replaceChar(Str& inOut, const char pattern, const char replacement) {
  for (char& letter : inOut) {
    if (letter == pattern) {
      letter = replacement;
    }
  }
}

void replaceForwardWithBackSlashes(Str& path) { replaceChar(path, kForwardSlash, kBackSlash); }

std::optional<Str> ip6ToValidHostname(const StrView& host) {
  if (!kIp6HostRegex.match(host)) {
    return std::nullopt;
  }
  Str ip6HostName;
  ip6HostName = kIp6HostRegex.lastMatchGroup(1);
  ip6HostName += kIp6HostSuffix;
  replaceChar(ip6HostName, kColon, kHyphen);
  return ip6HostName;
}

[[nodiscard]] Str pathToUrl(const StrView& windowsPath) {
  // Precondition.
  assert(!windowsPath.empty());

  ada::url url;
  // Note: url.set_protocol(...) is a no-op, see https://github.com/ada-url/ada/issues/573
  url.type = ada::scheme::FILE;

  if (const std::optional<UncDetails> uncDetails = extractUncDetails(windowsPath)) {
    validateUncPath(windowsPath, *uncDetails);
    setUrlHostFromUncPath(windowsPath, *uncDetails, url);
    setUrlPathFromUncPath(windowsPath, *uncDetails, url);
  } else {
    validateDrivePath(windowsPath);
    // Must explicitly set empty host to get `file://` rather than
    // `file:`.
    url.set_host("");
    setUrlPathFromDrivePath(windowsPath, url);
  }

  return url.get_href();
}

[[nodiscard]] Str pathFromUrl(const StrView url) {
  ada::result<ada::url_aggregator> adaUrl = ada::parse(url);
  if (!adaUrl) {
    throwError("Failed to parse URL", url);
  }

  Str windowsPath;

  const StrView host = adaUrl->get_host();
  const StrView encodedPath = adaUrl->get_pathname();

  StrView trimmedPath = encodedPath;
  if (host.empty() && !encodedPath.empty()) {
    trimmedPath = encodedPath.substr(1);
  }

  Str decodedPath = ada::unicode::percent_decode(trimmedPath, trimmedPath.find(kPercent));

  // TODO(DF): Ordering of validation to satisfy error priority of
  // swift-url test cases.

  if (host.empty()) {
    if (!isAbsoluteDrivePath(decodedPath)) {
      throwError(kErrorRelativePath, url);
    }
  }

  if (containsNullByte(decodedPath)) {
    throwError(kErrorNullByte, url);
  }
  if (containsPercentEncodedSlash(encodedPath)) {
    throwError(kErrorEncodedSeparator, url);
  }

  if (!host.empty()) {
    if (isInvalidHostname(host)) {
      throwError(kErrorUnsupportedHostname, url);
    }

    windowsPath += kDoubleBackSlash;

    if (const auto ip6Host = ip6ToValidHostname(host)) {
      windowsPath += *ip6Host;
    } else {
      windowsPath += host;
    }
  }

  decodedPath = replaceTrailingForwardSlashesInPathSegments(decodedPath);
  windowsPath += decodedPath;

  replaceForwardWithBackSlashes(windowsPath);

  return windowsPath;
}
}  // namespace win

namespace posix {

// E.g. `file://server/path
constexpr StrView kErrorNonLocal = "Unsupported non-local file";

Regex kUpwardsTraversalRegex{R"((^|/)\.\.(/|$))"};
/**
 * Augment default percent encoded set for paths.
 *
 * From swift-url's` `POSIXPathEncodeSet` docstring:
 *
 * - The '%' sign itself. Filesystem paths do not contain
 * percent-encoding, and any character sequences which look like
 * percent-encoding are just coincidences.
 * - Backslashes (`\`). They are allowed in POSIX paths and are not
 * separators.
 * - Colons (`:`) and vertical bars (`|`). These are sometimes
 * interpreted as Windows drive letter delimiters, which POSIX paths
 * obviously do not have.
 */
constexpr std::array kPercentEncodeCharacterSet = [] {
  constexpr std::uint8_t kByteSize = 8;
  constexpr std::size_t kArrSize = 32;  // 0xFF/kByteSize + 1;

  constexpr std::uint8_t kPercentHex = 0x25;
  constexpr std::uint8_t kBackSlashHex = 0x5C;
  constexpr std::uint8_t kColonHex = 0x3A;
  constexpr std::uint8_t kVerticalBarHex = 0x7C;

  std::array<uint8_t, kArrSize> charSet{};
  // Copy Ada's default %-encode set for URL path components.
  for (std::size_t idx = 0; idx < charSet.size(); ++idx) {
    charSet[idx] = ada::character_sets::PATH_PERCENT_ENCODE[idx];
  }
  // Augment the %-encode set with additional characters.
  for (std::uint8_t charCode : {kPercentHex, kBackSlashHex, kColonHex, kVerticalBarHex}) {
    charSet[charCode / kByteSize] |= static_cast<std::uint8_t>(1 << (charCode % kByteSize));
  }

  return charSet;
}();

[[nodiscard]] bool containsUpwardsTraversal(const StrView str) {
  return kUpwardsTraversalRegex.match(str);
}

[[nodiscard]] bool startsWithForwardSlash(const StrView str) {
  // Precondition
  assert(!str.empty());
  return str.front() == '/';
}

Str collapseForwardSlashes(const StrView& path) {
  if (path.size() <= 2) {
    return Str{path};
  }
  Str normalisedPath;
  normalisedPath.reserve(path.size());
  if (path[0] == kForwardSlash && path[1] == kForwardSlash && path[2] != kForwardSlash) {
    // Apparently two leading `/`s are implementation defined, so should
    // be retained. Any more than two should be collapsed to one.
    const StrView pathView{path};
    normalisedPath += pathView.substr(0, 2);
    normalisedPath += replaceTrailingForwardSlashesInPathSegments(pathView.substr(2));
  } else {
    normalisedPath = replaceTrailingForwardSlashesInPathSegments(path);
  }
  return normalisedPath;
}

[[nodiscard]] Str pathToUrl(const StrView linuxPath) {
  // Precondition.
  assert(!linuxPath.empty());

  if (containsUpwardsTraversal(linuxPath)) {
    throwError(kErrorUpwardsTraversal, linuxPath);
  }
  if (!startsWithForwardSlash(linuxPath)) {
    throwError(kErrorRelativePath, linuxPath);
  }

  ada::url adaUrl;
  adaUrl.type = ada::scheme::FILE;
  // Must explicitly set empty host to get `file://` rather than
  // `file:`.
  adaUrl.set_host("");

  Str processedPath;
  // Ada will %-encode, but with a more limited set that we want.
  ada::unicode::percent_encode<false>(linuxPath, kPercentEncodeCharacterSet.data(), processedPath);

  if (processedPath.empty()) {
    processedPath = linuxPath;
  }
  // Collapse multiple `/` to single, except at the beginning, where
  // `//` is valid.
  processedPath = collapseForwardSlashes(processedPath);

  if (!adaUrl.set_pathname(processedPath)) {
    throwError("Failed to create URL path from UNC path", linuxPath);
  }

  return adaUrl.get_href();
}

[[nodiscard]] Str pathFromUrl(const StrView url) {
  ada::result<ada::url_aggregator> adaUrl = ada::parse(url);
  if (!adaUrl) {
    throwError("Failed to parse URL", url);
  }

  if (!adaUrl->get_host().empty()) {
    throwError(kErrorNonLocal, url);
  }

  const StrView path = adaUrl->get_pathname();

  if (containsPercentEncodedForwardSlash(path)) {
    throwError(kErrorEncodedSeparator, url);
  }

  const Str decodedPath = ada::unicode::percent_decode(path, path.find(kPercent));

  if (containsNullByte(decodedPath)) {
    throwError(kErrorNullByte, url);
  }

  Str normalisedPath = collapseForwardSlashes(decodedPath);

  return normalisedPath;
}
}  // namespace posix

}  // namespace

Str pathToUrl(const StrView absolutePath, PathType pathType) {
  if (absolutePath.empty()) {
    throw errors::InputValidationException(Str{kErrorEmptyPath});
  }

  if (containsNullByte(absolutePath)) {
    throw errors::InputValidationException(Str{kErrorNullByte});
  }

  pathType = resolveSystemPathType(pathType);

  return pathType == PathType::kWindows ? win::pathToUrl(absolutePath)
                                        : posix::pathToUrl(absolutePath);
}

Str pathFromUrl(const StrView fileUrl, PathType pathType) {
  if (!isFileUrl(fileUrl)) {
    throwError(kErrorNotAFileUrl, fileUrl);
  }
  pathType = resolveSystemPathType(pathType);
  return pathType == PathType::kWindows ? win::pathFromUrl(fileUrl) : posix::pathFromUrl(fileUrl);
}

}  // namespace utils
}  // namespace OPENASSETIO_CORE_ABI_VERSION
}  // namespace openassetio
