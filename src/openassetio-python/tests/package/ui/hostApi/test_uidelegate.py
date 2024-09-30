#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd
from unittest import mock

import pytest
from openassetio.trait import TraitsData
from openassetio.ui.hostApi import UIDelegate


class Test_UIDelegate_identifier:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface
    ):
        mock_ui_delegate_interface.mock.identifier.return_value = "mock_identifier"

        assert uiDelegate.identifier() == "mock_identifier"


class Test_UIDelegate_displayName:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface
    ):
        mock_ui_delegate_interface.mock.displayName.return_value = "mock_displayName"

        assert uiDelegate.displayName() == "mock_displayName"


class Test_UIDelegate_info:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface
    ):
        mock_ui_delegate_interface.mock.info.return_value = {"a": "b"}

        assert uiDelegate.info() == {"a": "b"}


class Test_UIDelegate_settings:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface, a_host_session
    ):
        mock_ui_delegate_interface.mock.settings.return_value = {"c": "d"}

        assert uiDelegate.settings() == {"c": "d"}

        mock_ui_delegate_interface.mock.settings.assert_called_once_with(a_host_session)


class Test_UIDelegate_initialize:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface, a_host_session
    ):
        uiDelegate.initialize({"c": "d"})

        mock_ui_delegate_interface.mock.initialize.assert_called_once_with(
            {"c": "d"}, a_host_session
        )


class Test_UIDelegate_populateUI:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface, a_host_session
    ):

        class Container:
            pass

        class NativeData:
            pass

        class OutputWidget:
            pass

        container = Container()
        uiTraits = TraitsData({"ui_trait"})
        entityTraits = TraitsData({"entity_trait"})
        nativeData = NativeData()
        expected_output = OutputWidget()

        mock_ui_delegate_interface.mock.populateUI.return_value = expected_output

        actual_output = uiDelegate.populateUI(container, uiTraits, entityTraits, nativeData)

        mock_ui_delegate_interface.mock.populateUI.assert_called_once_with(
            container, uiTraits, entityTraits, nativeData, a_host_session
        )
        assert actual_output is expected_output


@pytest.fixture
def uiDelegate(mock_ui_delegate_interface, a_host_session):
    return UIDelegate(mock_ui_delegate_interface, a_host_session)
