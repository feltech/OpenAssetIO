#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd
from unittest import mock

import pytest

from openassetio import Context, EntityReference
from openassetio.ui import UIDelegateState
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
        # setup

        class NativeData:
            pass

        ui_traits = TraitsData({"ui"})
        native_data = NativeData()
        entity_ref = EntityReference("a")
        entity_traits = TraitsData({"entity"})
        relationship_traits = TraitsData({"relationship"})
        state_changed_callback = mock.Mock()
        initial_state = UIDelegateState(
            native_data, [entity_ref], [entity_traits], relationship_traits
        )
        context = Context()
        dispatch_state_callback = mock.Mock()
        mock_ui_delegate_interface.mock.populateUI.return_value = dispatch_state_callback

        # action

        actual_dispatch_state_callback = uiDelegate.populateUI(
            ui_traits, initial_state, context, state_changed_callback
        )

        # confirm

        mock_ui_delegate_interface.mock.populateUI.assert_called_once_with(
            ui_traits, initial_state, context, a_host_session, mock.ANY
        )

        # Check callbacks.
        manager_updated_state = UIDelegateState(
            native_data, [EntityReference("b")], [entity_traits], relationship_traits
        )
        host_updated_state = UIDelegateState(
            native_data, [entity_ref], [TraitsData({"entity", "image"})], relationship_traits
        )

        actual_dispatch_state_callback(host_updated_state)
        dispatch_state_callback.assert_called_once_with(host_updated_state)

        actual_state_changed_callback = mock_ui_delegate_interface.mock.populateUI.call_args[0][4]
        actual_state_changed_callback(manager_updated_state)
        state_changed_callback.assert_called_once_with(manager_updated_state)


@pytest.fixture
def uiDelegate(mock_ui_delegate_interface, a_host_session):
    return UIDelegate(mock_ui_delegate_interface, a_host_session)
