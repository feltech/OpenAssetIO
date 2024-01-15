#
#   Copyright 2023 The Foundry Visionmongers Ltd
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
"""
Tests that cover the openassetio.utils namespace.
"""
import collections
import json
import os
import re
from pathlib import Path

# pylint: disable=invalid-name,redefined-outer-name, too-few-public-methods
# pylint: disable=missing-class-docstring,missing-function-docstring
import pytest

from openassetio import utils
from openassetio.errors import InputValidationException
from openassetio.utils import PathType


URLMap = collections.namedtuple("URLMap", ("path_type", "path", "url"))

relative_path_error_message = "Relative paths cannot be converted to a URL ('{}')"
non_file_scheme_error_message = "Must be a 'file' scheme URL ('{}')"
server_invalid_on_posix_error_message = (
    "Server name components are invalid on POSIX for file scheme URLs ('{}')"
)

error_messages = {
    "relative-path": "Path is relative ('{}')",
    "empty-input": "Path is empty",
    "invalid-namespaced-path": "Path is ill-formed ('{}')",
    "invalid-hostname": "Path references an invalid hostname ('{}')",
    "unsupported-namespaced-path": "Unsupported Win32 namespaced path ('{}')",
    "upwards-traversal": "Path contains upwards traversal ('{}')",
    "null-byte": "Path contains NULL bytes",
    "unsupported-non-local-file": "Unsupported non-local file ('{}')",
    "not-a-file-url": "Not a file URL ('{}')",
    "encoded-separator": "Percent-encoded path separator ('{}')",
    "unsupported-hostname": "Unsupported hostname ('{}')"
}

test_data = (
    # Standard POSIX path with some %-encoding.
    URLMap(
        PathType.kPOSIX,
        "/Testing/exr/cathedral/cathedral_withdisparity_%V.####.exr",
        "file:///Testing/exr/cathedral/cathedral_withdisparity_%25V.%23%23%23%23.exr",
    ),
    # Reject non-file URLs.
    URLMap(
        PathType.kPOSIX,
        InputValidationException(
            non_file_scheme_error_message.format("filethisisnot:///some/path")
        ),
        "filethisisnot:///some/path",
    ),
    URLMap(
        PathType.kWindows,
        InputValidationException(
            non_file_scheme_error_message.format("filethisisnot://server/some/path")
        ),
        "filethisisnot://server/some/path",
    ),
    # Reject relative paths.
    URLMap(
        PathType.kPOSIX,
        "relative_path",
        InputValidationException(relative_path_error_message.format("relative_path")),
    ),
    URLMap(
        PathType.kWindows,
        "relative_path",
        InputValidationException(relative_path_error_message.format("relative_path")),
    ),
    URLMap(
        PathType.kPOSIX,
        "./relative_path",
        InputValidationException(relative_path_error_message.format("./relative_path")),
    ),
    URLMap(
        PathType.kWindows,
        r".\relative_path",
        InputValidationException(relative_path_error_message.format(r".\relative_path")),
    ),
    URLMap(
        PathType.kWindows,
        r"C:relative_path",
        InputValidationException(relative_path_error_message.format(r"C:relative_path")),
    ),
    # Leading `/` differences
    URLMap(
        PathType.kPOSIX,
        "/D:/pretending/to/be/windows",
        "file:///D:/pretending/to/be/windows",
    ),
    URLMap(
        PathType.kWindows,
        "D:/actually/am/windows",
        "file:///D:/actually/am/windows",
    ),
    URLMap(
        PathType.kPOSIX,
        "D:/pretending/to/be/windows",
        InputValidationException(
            relative_path_error_message.format("D:/pretending/to/be/windows")
        ),
    ),
    # URL reserved characters in path.
    URLMap(
        PathType.kPOSIX,
        "/user:pass@server/path?query=param#frag ment",
        "file:///user:pass@server/path%3Fquery=param%23frag%20ment",
    ),
    URLMap(
        PathType.kWindows,
        r"C:\user:pass@server\path?query=param#frag ment",
        r"file:///C:/user:pass@server/path%3Fquery=param%23frag%20ment",
    ),
    # Unicode path component
    URLMap(
        PathType.kPOSIX,
        "/äi /am/an awkäd/🎃 uniĆø∂e/path",
        "file:///%C3%A4i%20/am/an%20awk%C3%A4d/%F0%9F%8E%83%20uni%C4%86%C3%B8%E2%88%82e/path",
    ),
    URLMap(
        PathType.kWindows,
        r"C:\äi \am\an awkäd\🎃 uniĆø∂e\path",
        "file:///C:/%C3%A4i%20/am/an%20awk%C3%A4d/%F0%9F%8E%83%20uni%C4%86%C3%B8%E2%88%82e/path",
    ),
    # UNC server path
    URLMap(
        PathType.kWindows,
        r"\\laptop\My Documents\FileSchemeURIs.doc",
        "file://laptop/My%20Documents/FileSchemeURIs.doc",
    ),
    URLMap(
        PathType.kPOSIX,
        r"\\laptop\My Documents\FileSchemeURIs.doc",
        # `\` is just another character on POSIX
        InputValidationException(
            relative_path_error_message.format(r"\\laptop\My Documents\FileSchemeURIs.doc")
        ),
    ),
    # https://en.wikipedia.org/wiki/Administrative_share
    URLMap(
        PathType.kWindows,
        r"\\laptop\C$\My Documents\FileSchemeURIs.doc",
        "file://laptop/C$/My%20Documents/FileSchemeURIs.doc",
    ),
    URLMap(
        PathType.kPOSIX,
        InputValidationException(
            server_invalid_on_posix_error_message.format(
                "file://laptop/My%20Documents/FileSchemeURIs.doc"
            )
        ),
        "file://laptop/My%20Documents/FileSchemeURIs.doc",
    ),
    # UNC unicode path
    # TODO(DF): based on experimentation, Windows server/domain part
    #  cannot contain special characters (both NetBIOS and DNS variants)
    #  however Windows docs imply they can support unicode, but if so its
    #  probably locale-dependent so here just assumed we pass the server
    #  part verbatim with no encoding.
    URLMap(
        PathType.kWindows,
        r"\\l🎃ptop\My D🎃cuments\FileSchemeURIs.doc",
        # verbatim host, %-encode path.
        "file://l🎃ptop/My%20D%F0%9F%8E%83cuments/FileSchemeURIs.doc",
    ),
    # Windows paths using `/` instead of `\`.
    # TODO(DF): Verbatim encode (%-encode `/`)?  Or lose `/` information (as below)?
    URLMap(
        PathType.kWindows,
        r"C:/Documents and Settings/FileSchemeURIs.doc",
        # Loses `/` usage
        "file:///C:/Documents%20and%20Settings/FileSchemeURIs.doc",
    ),
    # Windows paths using mixed `/` and `\`.
    URLMap(
        PathType.kWindows,
        r"C:/Documents and Settings\FileSchemeURIs.doc",
        # Loses `/` usage info
        "file:///C:/Documents%20and%20Settings/FileSchemeURIs.doc",
    ),
    # Extended UNC - i.e. verbatim
    # TODO(DF): `file://` vs. `file:///; or treat as other UNC but
    #  without `/` to `\` conversion?
    # TODO(DF): auto prefix with `\\?\` when converting long `file://`
    #  urls?
    # https://learn.microsoft.com/en-us/windows/win32/fileio/naming-a-file#win32-file-namespaces
    URLMap(
        PathType.kWindows,
        r"\\?\C:\l🎃ptop\Documents and Settings\some/dodgy?\../file.doc",
        f"file://%5C%3FC%3Al%F0%9F%8E%83ptopDocuments%20and%20Settingssome%2Fdodgy%3F..%2Ffile.doc",
    ),
    URLMap(
        PathType.kWindows,
        r"\\?\UNC\l🎃ptop\Documents and Settings\some/dodgy?\..\/file.doc",
        f"file://%5C%3FUNCl%F0%9F%8E%83ptopDocuments%20and%20Settingssome%2Fdodgy%3F..%2Ffile.doc",
    ),
)


class Test_PathType_enum:
    def test_expected_constants_are_unique_and_exhaustive(self):
        for i, enum in enumerate((PathType.kSystem, PathType.kPOSIX, PathType.kWindows)):
            assert int(enum) == i


class Test_pathToFileURL_pathFromFileURL_roundtrip:
    @pytest.mark.parametrize("url_map", test_data)
    @pytest.mark.skip("TODO(DF): focussing on tests from upstream JSON DB")
    def test(self, url_map):
        os_path_type = PathType.kWindows if os.name == "nt" else PathType.kPOSIX

        # Happy.
        if isinstance(url_map.path, str) and isinstance(url_map.url, str):
            assert utils.pathToUrl(url_map.path, url_map.path_type) == url_map.url
            assert utils.pathFromUrl(url_map.url, url_map.path_type) == url_map.path

            if os_path_type == url_map.path_type:
                assert utils.pathToUrl(url_map.path, PathType.kSystem) == url_map.url
                assert utils.pathFromUrl(url_map.url, PathType.kSystem) == url_map.path
                assert utils.pathToUrl(url_map.path) == url_map.url
                assert utils.pathFromUrl(url_map.url) == url_map.path

        # Bad path.
        elif isinstance(url_map.path, str) and isinstance(url_map.url, Exception):

            with pytest.raises(type(url_map.url), match=exc_to_regex(url_map.url)):
                _ = utils.pathToUrl(url_map.path, url_map.path_type)

            if os_path_type == url_map.path_type:
                with pytest.raises(type(url_map.url), match=exc_to_regex(url_map.url)):
                    utils.pathToUrl(url_map.path, PathType.kSystem)
                with pytest.raises(type(url_map.url), match=exc_to_regex(url_map.url)):
                    utils.pathToUrl(url_map.path)

        # Bad URL.
        elif isinstance(url_map.path, Exception) and isinstance(url_map.url, str):
            with pytest.raises(type(url_map.path), match=exc_to_regex(url_map.path)):
                _unexpected = utils.pathFromUrl(url_map.url, url_map.path_type)

            if os_path_type == url_map.path_type:
                with pytest.raises(type(url_map.path), match=exc_to_regex(url_map.path)):
                    utils.pathFromUrl(url_map.url, PathType.kSystem)
                with pytest.raises(type(url_map.path), match=exc_to_regex(url_map.path)):
                    utils.pathFromUrl(url_map.url)
        else:
            raise RuntimeError("Unhandled URL mapping")


class Test_pathToUrl:
    def test_posix(self, subtests, file_path_to_url_json):
        for case in file_path_to_url_json:
            with subtests.test(msg=case["comment"], path=case["file_path"], url=case["URL_posix"]):
                self.assert_valid_url_map(
                    URLMap(
                        PathType.kPOSIX,
                        case["file_path"],
                        str_or_error(case["URL_posix"], case["file_path"]),
                    )
                )

    def test_windows(self, subtests, file_path_to_url_json):
        for case in file_path_to_url_json:
            with subtests.test(
                msg=case["comment"], path=case["file_path"], url=case["URL_windows"]
            ):
                self.assert_valid_url_map(
                    URLMap(
                        PathType.kWindows,
                        case["file_path"],
                        str_or_error(case["URL_windows"], case["file_path"]),
                    )
                )

    def assert_valid_url_map(self, url_map):
        os_path_type = PathType.kWindows if os.name == "nt" else PathType.kPOSIX

        # Happy.
        if isinstance(url_map.path, str) and isinstance(url_map.url, str):
            assert utils.pathToUrl(url_map.path, url_map.path_type) == url_map.url

            if os_path_type == url_map.path_type:
                assert utils.pathToUrl(url_map.path, PathType.kSystem) == url_map.url
                assert utils.pathToUrl(url_map.path) == url_map.url

        # Bad path.
        elif isinstance(url_map.path, str) and isinstance(url_map.url, Exception):

            with pytest.raises(type(url_map.url), match=exc_to_regex(url_map.url)):
                _unexpected = utils.pathToUrl(url_map.path, url_map.path_type)

            if os_path_type == url_map.path_type:
                with pytest.raises(type(url_map.url), match=exc_to_regex(url_map.url)):
                    utils.pathToUrl(url_map.path, PathType.kSystem)
                with pytest.raises(type(url_map.url), match=exc_to_regex(url_map.url)):
                    utils.pathToUrl(url_map.path)
        else:
            raise RuntimeError("Unhandled URL mapping")


class Test_pathFromUrl:
    def test_posix(self, subtests, url_to_file_path_json):
        for case in url_to_file_path_json:
            with subtests.test(msg=case["comment"], path=case["file_path_posix"], url=case["URL"]):
                self.assert_valid_url_map(
                    URLMap(
                        PathType.kPOSIX,
                        str_or_error(case["file_path_posix"], case["URL"]),
                        case["URL"],
                    )
                )

    def test_windows(self, subtests, url_to_file_path_json):
        for case in url_to_file_path_json:
            with subtests.test(
                msg=case["comment"], path=case["file_path_windows"], url=case["URL"]
            ):
                self.assert_valid_url_map(
                    URLMap(
                        PathType.kWindows,
                        str_or_error(case["file_path_windows"], case["URL"]),
                        case["URL"],
                    )
                )

    def assert_valid_url_map(self, url_map):
        os_path_type = PathType.kWindows if os.name == "nt" else PathType.kPOSIX

        # Happy.
        if isinstance(url_map.path, str) and isinstance(url_map.url, str):
            assert utils.pathFromUrl(url_map.url, url_map.path_type) == url_map.path

            if os_path_type == url_map.path_type:
                assert utils.pathFromUrl(url_map.url, PathType.kSystem) == url_map.path
                assert utils.pathFromUrl(url_map.url) == url_map.path

        # Bad URL.
        elif isinstance(url_map.path, Exception) and isinstance(url_map.url, str):
            with pytest.raises(type(url_map.path), match=exc_to_regex(url_map.path)):
                _ = utils.pathFromUrl(url_map.url, url_map.path_type)

            if os_path_type == url_map.path_type:
                with pytest.raises(type(url_map.path), match=exc_to_regex(url_map.path)):
                    utils.pathFromUrl(url_map.url, PathType.kSystem)
                with pytest.raises(type(url_map.path), match=exc_to_regex(url_map.path)):
                    utils.pathFromUrl(url_map.url)

        else:
            raise RuntimeError("Unhandled URL mapping")


def exc_to_regex(exc):
    return re.escape(str(exc))


def str_or_error(maybe_error, path_or_url):
    if isinstance(maybe_error, dict):
        return InputValidationException(
            error_messages[maybe_error["failure-reason"]].format(path_or_url)
        )
    return maybe_error


@pytest.fixture
def file_path_to_url_json(file_url_path_tests_json):
    return (
        case for case in file_url_path_tests_json["file_path_to_url"] if "__section__" not in case
    )


@pytest.fixture
def url_to_file_path_json(file_url_path_tests_json):
    return (
        case for case in file_url_path_tests_json["url_to_file_path"] if "__section__" not in case
    )


@pytest.fixture(scope="session")
def file_url_path_tests_json():
    with open(
        Path(__file__).parent / "resources" / "file_url_path_tests.json", "r", encoding="utf-8"
    ) as json_file:
        return json.load(json_file)
