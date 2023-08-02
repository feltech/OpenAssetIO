#
#   Copyright 2022 The Foundry Visionmongers Ltd
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
Tests of C++ binding utilities, which require dummy C++ classes that
shouldn't be included in the main sources.
"""

# pylint: disable=missing-function-docstring

from openassetio import _openassetio_test  # pylint: disable=no-name-in-module


def test_useUserDefinedType():
    assert _openassetio_test.DefaultArged().useUserDefinedType() == 1
    assert _openassetio_test.DefaultArged().useUserDefinedType() == 1


def test_useSet():
    assert _openassetio_test.DefaultArged().useSet() == 1
    assert _openassetio_test.DefaultArged().useSet() == 1
