#
#   Copyright 2025 The Foundry Visionmongers Ltd
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
# pylint: disable=invalid-name,redefined-outer-name
# pylint: disable=too-few-public-methods
# pylint: disable=missing-class-docstring,missing-function-docstring
"""
Tests for the Python convenience classes UIDelegateRequest and
UIDelegateState.
"""

from unittest import mock


from openassetio import EntityReference
from openassetio.trait import TraitsData
from openassetio.ui.UIDelegateState import UIDelegateState, UIDelegateRequest


class Test_UIDelegateRequest_init:

    def test_when_default_constructed_then_members_are_empty(self):
        ui_request = UIDelegateRequest()

        assert ui_request.nativeData() is None
        assert ui_request.entityReferences() == []
        assert ui_request.entityTraitsDatas() == []
        assert ui_request.stateChangedCallback() is None

    def test_when_members_set_on_construction_then_stores_members(self):
        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        state_changed_cb = mock.Mock()

        ui_request = UIDelegateRequest(
            nativeData=native_data,
            entityReferences=entity_refs,
            entityTraitsDatas=entity_traits,
            stateChangedCallback=state_changed_cb,
        )

        assert ui_request.entityReferences() == entity_refs
        assert ui_request.entityTraitsDatas() == entity_traits
        assert ui_request.nativeData() is native_data
        ui_request.stateChangedCallback()(UIDelegateState())
        state_changed_cb.assert_called_once()

    def test_when_members_set_externally_then_stores_members(self):
        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        state_changed_cb = mock.Mock()

        ui_request = UIDelegateRequest()
        ui_request.setNativeData(native_data)
        ui_request.setEntityReferences(entity_refs)
        ui_request.setEntityTraitsDatas(entity_traits)
        ui_request.setStateChangedCallback(state_changed_cb)

        assert ui_request.entityReferences() == entity_refs
        assert ui_request.entityTraitsDatas() == entity_traits
        assert ui_request.nativeData() is native_data
        ui_request.stateChangedCallback()(UIDelegateState())
        state_changed_cb.assert_called_once()


class Test_UIDelegateState_init:

    def test_when_default_constructed_then_members_are_empty(self):
        ui_state = UIDelegateState()
        assert ui_state.nativeData() is None
        assert ui_state.entityReferences() == []
        assert ui_state.entityTraitsDatas() == []
        assert ui_state.updateRequestCallback() is None

    def test_when_members_set_on_construction_then_stores_members(self):
        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        update_request_cb = mock.Mock()

        ui_state = UIDelegateState(
            nativeData=native_data,
            entityReferences=entity_refs,
            entityTraitsDatas=entity_traits,
            updateRequestCallback=update_request_cb,
        )

        assert ui_state.entityReferences() == entity_refs
        assert ui_state.entityTraitsDatas() == entity_traits
        assert ui_state.nativeData() is native_data
        ui_state.updateRequestCallback()(UIDelegateRequest())
        update_request_cb.assert_called_once()

    def test_when_state_members_set_externally_then_stores_members(self):
        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        update_request_cb = mock.Mock()

        ui_state = UIDelegateState()
        ui_state.setEntityReferences(entity_refs)
        ui_state.setEntityTraitsDatas(entity_traits)
        ui_state.setNativeData(native_data)
        ui_state.setUpdateRequestCallback(update_request_cb)

        assert ui_state.entityReferences() == entity_refs
        assert ui_state.entityTraitsDatas() == entity_traits
        assert ui_state.nativeData() is native_data
        ui_state.updateRequestCallback()(UIDelegateRequest())
        update_request_cb.assert_called_once()


class NativeData:
    """
    Stub object to use for arbitrary state `nativeData`.
    """
