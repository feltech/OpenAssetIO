#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd
import re

from openassetio import _openassetio
from openassetio.errors import InputValidationException

import pytest


class Test_UIDelegateState:
    def test_when_nativeData_non_pyobject_then_raises(self):
        ui = _openassetio._testutils.ui

        expected_error = re.escape(
            "Python UI delegates only accept Python objects: C++ type 'double' is not supported"
        )

        state = ui.createUIDelegateStateWithNonPyObjectNativeData()

        with pytest.raises(InputValidationException, match=expected_error):
            state.nativeData()

    def test_when_returns_raw_cpython_then_ok(self, a_host_session):
        ui = _openassetio._testutils.ui
        ui_delegate_state = ui.createUIDelegateStateWithRawCPythonNativeData()

        assert ui_delegate_state.nativeData() == 42
