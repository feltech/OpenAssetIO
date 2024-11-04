#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd
import sys
import weakref
from unittest import mock

import pytest

from openassetio import Context, EntityReference
from openassetio.ui import UIDelegateState, UIDelegateRequest, constants, access
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

        assert uiDelegate.info() == {"a": "b", constants.kInfoKey_PythonOnly: True}


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


class Test_UIDelegate_close:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface, a_host_session
    ):
        uiDelegate.close()

        mock_ui_delegate_interface.mock.close.assert_called_once_with(a_host_session)

    def test_when_destroyed_then_close_called(self, mock_ui_delegate_interface, a_host_session):
        UIDelegate(mock_ui_delegate_interface, a_host_session)

        mock_ui_delegate_interface.mock.close.assert_called_once_with(a_host_session)

    def test_when_destroyed_and_close_raises_then_exception_logged(
        self, mock_ui_delegate_interface, a_host_session, mock_logger
    ):
        error = RuntimeError("Boom!")
        mock_ui_delegate_interface.mock.close.side_effect = error

        UIDelegate(mock_ui_delegate_interface, a_host_session)

        args, kwargs = mock_logger.mock.log.call_args
        assert args[0] == mock_logger.Severity.kError
        assert args[1].startswith("Exception in destructor of UIDelegate: RuntimeError: Boom!")


class Test_UIDelegate_populateUI:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface, a_host_session
    ):
        # setup

        ui_traits = TraitsData({"ui"})
        native_data = NativeData()
        entity_ref = EntityReference("a")
        entity_traits = TraitsData({"entity"})
        relationship_traits = TraitsData({"relationship"})
        context = Context()
        state_changed_callback = mock.Mock()
        update_request_callback = mock.Mock()
        ui_access = access.UIAccess.kRead
        request_state = UIDelegateRequest(
            native_data,
            [entity_ref],
            [entity_traits],
            [relationship_traits],
            state_changed_callback,
        )
        expected_initial_ui_state = UIDelegateState(
            native_data, [entity_ref], [entity_traits], update_request_callback
        )
        mock_ui_delegate_interface.mock.populateUI.return_value = expected_initial_ui_state

        # action

        actual_initial_ui_state = uiDelegate.populateUI(
            ui_traits, ui_access, request_state, context
        )

        # confirm

        mock_ui_delegate_interface.mock.populateUI.assert_called_once_with(
            ui_traits, ui_access, request_state, context, a_host_session
        )
        assert actual_initial_ui_state is expected_initial_ui_state
        assert actual_initial_ui_state.nativeData is native_data
        assert actual_initial_ui_state.entityReferences == [entity_ref]
        assert actual_initial_ui_state.entityTraitsDatas == [entity_traits]  # Check callbacks.

        # setup

        updated_request_state = UIDelegateRequest(entityReferences=[EntityReference("b")])

        # action

        actual_initial_ui_state.updateRequestCallback(updated_request_state)

        # confirm

        update_request_callback.assert_called_once_with(mock.ANY)
        assert update_request_callback.call_args[0][0].entityReferences == [EntityReference("b")]

        # setup

        updated_ui_state = UIDelegateState(entityReferences=[EntityReference("c")])
        actual_request_state = mock_ui_delegate_interface.mock.populateUI.call_args[0][2]

        # action

        actual_request_state.stateChangedCallback(updated_ui_state)

        # confirm

        state_changed_callback.assert_called_once_with(mock.ANY)
        assert state_changed_callback.call_args[0][0].entityReferences == [EntityReference("c")]

    def test_refcount(self, uiDelegate, mock_ui_delegate_interface, a_host_session):
        # setup
        print("Starting test")

        ui_traits = TraitsData({"ui"})
        native_data = NativeData()
        entity_ref = EntityReference("a")
        entity_traits = TraitsData({"entity"})
        relationship_traits = TraitsData({"relationship"})
        context = Context()
        state_changed_callback = mock.Mock()
        update_request_callback = mock.Mock()
        ui_access = access.UIAccess.kRead
        request_state = UIDelegateRequest(
            native_data,
            [entity_ref],
            [entity_traits],
            [relationship_traits],
            state_changed_callback,
        )

        def create_state(*args, **kwargs):
            return_state = UIDelegateState()

            class A:
                pass

            print("Setting some_attr")
            return_state.some_attr = A()
            print("Setting nativeData")
            return_state.nativeData = A()
            print("Returning state")
            return return_state

        mock_ui_delegate_interface.mock.populateUI.side_effect = create_state

        # action

        print("Calling populateUI")
        actual_initial_ui_state = uiDelegate.populateUI(
            ui_traits, ui_access, request_state, context
        )

        # confirm

        # weakref_to_actual_state = weakref.ref(actual_initial_ui_state)
        print("Creating weakref")
        weakref_to_actual_nativeData = weakref.ref(actual_initial_ui_state.nativeData)

        # assert actual_initial_ui_state.some_attr
        print("Deleting UI state")
        del actual_initial_ui_state
        # assert weakref_to_actual_state() is None
        # assert weakref_to_actual_nativeData() is None


class Test_UIDelegate_uiPolicy:
    def test_wraps_the_corresponding_method_of_the_held_interface(
        self, uiDelegate, mock_ui_delegate_interface, a_host_session
    ):
        ui_traits = {"ui"}
        context = Context()
        ui_access = access.UIAccess.kRead
        expected_policy = TraitsData({"some_policy"})
        mock_ui_delegate_interface.mock.uiPolicy.return_value = expected_policy

        assert uiDelegate.uiPolicy(ui_traits, ui_access, context) == expected_policy

        mock_ui_delegate_interface.mock.uiPolicy.assert_called_once_with(
            ui_traits, ui_access, context, a_host_session
        )


class NativeData:
    pass


@pytest.fixture
def uiDelegate(mock_ui_delegate_interface, a_host_session):
    return UIDelegate(mock_ui_delegate_interface, a_host_session)
