#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd
import re

from openassetio import _openassetio
from openassetio.trait import TraitsData
from openassetio.errors import InputValidationException
from openassetio.ui.hostApi import UIDelegate
from openassetio.ui.managerApi import UIDelegateInterface

import pytest


class Test_UIDelegateInterface_populateUI:
    def test_when_returns_non_pyobject_then_raises(self, a_host_session):
        ui = _openassetio._testutils.ui
        ui_delegate_interface = ui.createUIDelegateInterfaceThatReturnsNonPyObjFromPopulateUI()

        expected_error = re.escape(
            "Python UI delegates only accept Python objects: C++ type 'float' is not supported"
        )

        with pytest.raises(InputValidationException, match=expected_error):
            ui_delegate_interface.populateUI(
                None, TraitsData(), TraitsData(), None, a_host_session
            )

    def test_when_returns_raw_cpython_then_ok(self, a_host_session):
        ui = _openassetio._testutils.ui
        ui_delegate_interface = ui.createUIDelegateInterfaceThatReturnsRawCPythonFromPopulateUI()
        result = ui_delegate_interface.populateUI(
            None, TraitsData(), TraitsData(), None, a_host_session
        )

        assert result == 42


class Test_UIDelegate_populateUI:
    def test_when_container_is_non_pyobject_then_raises(self, a_host_session):
        ui_delegate = UIDelegate(StubPythonUIDelegateInterface(), a_host_session)

        expected_error = re.escape(
            "Python UI delegates only accept Python objects: C++ type 'int' is not supported"
        )

        with pytest.raises(InputValidationException, match=expected_error):
            _openassetio._testutils.ui.callPopulateUIWithNonPyObjectContainer(ui_delegate)

    def test_when_native_data_is_non_pyobject_then_raises(self, a_host_session):
        ui_delegate = UIDelegate(StubPythonUIDelegateInterface(), a_host_session)

        expected_error = re.escape(
            "Python UI delegates only accept Python objects: C++ type 'double' is not supported"
        )

        with pytest.raises(InputValidationException, match=expected_error):
            _openassetio._testutils.ui.callPopulateUIWithNonPyObjectNativeData(ui_delegate)


class StubPythonUIDelegateInterface(UIDelegateInterface):
    def identifier(self):
        return "org.openassetio.test.stub.uiDelegate.python"

    def displayName(self):
        return "Stub Python UI Delegate"

    def poplateUI(self, container, uiTraits, entityTraits, nativeData, hostSession):
        return None
