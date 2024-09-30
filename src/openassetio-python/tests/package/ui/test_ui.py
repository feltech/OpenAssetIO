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


import gc
import sys
import weakref
from unittest import mock


from openassetio import EntityReference
from openassetio.trait import TraitsData
from openassetio.ui import UIDelegateState, UIDelegateRequest


class NativeData:
    pass


class Test_UIDelegateState_init:

    def test_when_default_constructed_then_members_are_empty(self):
        ui_state = UIDelegateState()
        assert ui_state.nativeData() is None
        assert ui_state.entityReferences() == []
        assert ui_state.entityTraitsDatas() == []
        assert ui_state.updateRequestCallback() is None

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

    def test_when_state_nativeData_destroyed_then_reference_invalid(self):
        ui_state = UIDelegateState()
        ui_state.setNativeData(NativeData())
        weak_native_data = weakref.ref(ui_state.nativeData())

        assert weak_native_data() is not None
        del ui_state
        assert weak_native_data() is None

    def test_circular_ref(self):
        def fn():
            def state_cb(request):
                assert state

            state = UIDelegateState()
            state.setUpdateRequestCallback(state_cb)
            return state

        state = fn()
        weak_state = weakref.ref(state)
        state.updateRequestCallback()(UIDelegateRequest())
        del state
        gc.collect()
        assert weak_state() is None


class Test_UIDelegateRequest_init:

    def test_when_default_constructed_then_members_are_empty(self):
        ui_request = UIDelegateRequest()

        assert ui_request.nativeData() is None
        assert ui_request.entityReferences() == []
        assert ui_request.entityTraitsDatas() == []
        assert ui_request.stateChangedCallback() is None

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

    def test_when_nativeData_destroyed_then_reference_invalid(self):
        ui_request = UIDelegateRequest()
        ui_request.setNativeData(NativeData())
        weak_native_data = weakref.ref(ui_request.nativeData())

        assert weak_native_data() is not None
        del ui_request
        assert weak_native_data() is None
