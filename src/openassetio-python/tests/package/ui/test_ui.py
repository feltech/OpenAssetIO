#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd
from unittest import mock

#
#   Copyright 2024 The Foundry Visionmongers Ltd
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

from openassetio import EntityReference
from openassetio.trait import TraitsData
from openassetio.ui import UIDelegateState, UIDelegateRequest


class NativeData:
    pass


class Test_UIDelegateState_init:

    def test_when_default_constructed_then_members_are_empty(self):
        ui_state = UIDelegateState()

        assert ui_state.nativeData is None
        assert ui_state.entityReferences == []
        assert ui_state.entityTraitsDatas == []
        assert ui_state.updateRequestCallback is None

    def test_when_constructed_with_positional_args_then_stores_members(self):

        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        update_request_cb = mock.Mock()

        ui_state = UIDelegateState(native_data, entity_refs, entity_traits, update_request_cb)

        assert ui_state.nativeData is native_data
        assert ui_state.entityReferences == entity_refs
        assert ui_state.entityTraitsDatas == entity_traits
        ui_state.updateRequestCallback(UIDelegateRequest())
        update_request_cb.assert_called_once()

    def test_when_constructed_with_kwargs_then_stores_members(self):

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

        assert ui_state.nativeData is native_data
        assert ui_state.entityReferences == entity_refs
        assert ui_state.entityTraitsDatas == entity_traits
        ui_state.updateRequestCallback(UIDelegateRequest())
        update_request_cb.assert_called_once()

    def test_when_members_set_externally_then_stores_members(self):
        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        update_request_cb = mock.Mock()

        ui_state = UIDelegateState()
        ui_state.entityReferences = entity_refs
        ui_state.entityTraitsDatas = entity_traits
        ui_state.nativeData = native_data
        ui_state.updateRequestCallback = update_request_cb

        assert ui_state.entityReferences == entity_refs
        assert ui_state.entityTraitsDatas == entity_traits
        assert ui_state.nativeData is native_data
        ui_state.updateRequestCallback(UIDelegateRequest())
        update_request_cb.assert_called_once()


class Test_UIDelegateRequest_init:

    def test_when_default_constructed_then_members_are_empty(self):
        ui_request = UIDelegateRequest()

        assert ui_request.nativeData is None
        assert ui_request.entityReferences == []
        assert ui_request.entityTraitsDatas == []
        assert ui_request.relationshipTraitsDatas == []
        assert ui_request.stateChangedCallback is None

    def test_when_constructed_with_positional_args_then_stores_members(self):

        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        relationship_traits = [TraitsData({"relationship"})]
        state_changed_cb = mock.Mock()

        ui_request = UIDelegateRequest(
            native_data, entity_refs, entity_traits, relationship_traits, state_changed_cb
        )

        assert ui_request.nativeData is native_data
        assert ui_request.entityReferences == entity_refs
        assert ui_request.entityTraitsDatas == entity_traits
        assert ui_request.relationshipTraitsDatas == relationship_traits
        ui_request.stateChangedCallback(UIDelegateState())
        state_changed_cb.assert_called_once()

    def test_when_constructed_with_kwargs_then_stores_members(self):

        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        relationship_traits = [TraitsData({"relationship"})]
        state_changed_cb = mock.Mock()

        ui_request = UIDelegateRequest(
            nativeData=native_data,
            entityReferences=entity_refs,
            entityTraitsDatas=entity_traits,
            stateChangedCallback=state_changed_cb,
            relationshipTraitsDatas=relationship_traits,
        )

        assert ui_request.nativeData is native_data
        assert ui_request.entityReferences == entity_refs
        assert ui_request.entityTraitsDatas == entity_traits
        assert ui_request.relationshipTraitsDatas == relationship_traits
        ui_request.stateChangedCallback(UIDelegateState())
        state_changed_cb.assert_called_once()

    def test_when_members_set_externally_then_stores_members(self):
        native_data = NativeData()
        entity_refs = [EntityReference("a")]
        entity_traits = [TraitsData({"entity"})]
        relationship_traits = [TraitsData({"relationship"})]
        state_changed_cb = mock.Mock()

        ui_request = UIDelegateRequest()
        ui_request.nativeData = native_data
        ui_request.entityReferences = entity_refs
        ui_request.entityTraitsDatas = entity_traits
        ui_request.relationshipTraitsDatas = relationship_traits
        ui_request.stateChangedCallback = state_changed_cb

        assert ui_request.entityReferences == entity_refs
        assert ui_request.entityTraitsDatas == entity_traits
        assert ui_request.nativeData is native_data
        assert ui_request.relationshipTraitsDatas == relationship_traits
        ui_request.stateChangedCallback(UIDelegateState())
        state_changed_cb.assert_called_once()
