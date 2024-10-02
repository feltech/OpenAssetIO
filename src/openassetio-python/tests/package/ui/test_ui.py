#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd

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
from openassetio.ui import UIDelegateState


class Test_UIDelegateState_init:
    def test_when_constructed_with_members_then_stores_members(self):
        entity_refs = [EntityReference("a"), EntityReference("b")]
        entity_traits = [TraitsData({"entity"})]
        relationship_traits = TraitsData({"relationship"})
        native_data = object()

        ui_state = UIDelegateState(native_data, entity_refs, entity_traits, relationship_traits)

        assert ui_state.entityReferences == entity_refs
        assert ui_state.entityTraitsDatas == entity_traits
        assert ui_state.relationshipTraitsData is relationship_traits
        assert ui_state.nativeData is native_data

    def test_when_members_set_externally_then_stores_members(self):
        entity_refs = [EntityReference("a"), EntityReference("b")]
        entity_traits = [TraitsData({"entity"})]
        relationship_traits = TraitsData({"relationship"})
        native_data = object()

        ui_state = UIDelegateState()
        ui_state.entityReferences = entity_refs
        ui_state.entityTraitsDatas = entity_traits
        ui_state.relationshipTraitsData = relationship_traits
        ui_state.nativeData = native_data

        assert ui_state.entityReferences == entity_refs
        assert ui_state.entityTraitsDatas == entity_traits
        assert ui_state.relationshipTraitsData is relationship_traits
        assert ui_state.nativeData is native_data


class Test_UIDelegateState_eq:
    def test_when_states_are_equivalent_then_equality_is_true(self):
        entity_refs = [EntityReference("a"), EntityReference("b")]
        entity_traits = [TraitsData({"entity"})]
        relationship_traits = TraitsData({"relationship"})
        native_data = 123

        ui_state_1 = UIDelegateState(native_data, entity_refs, entity_traits, relationship_traits)
        ui_state_2 = UIDelegateState(native_data, entity_refs, entity_traits, relationship_traits)

        assert ui_state_1 == ui_state_2

    def test_when_states_are_not_equivalent_then_equality_is_false(self):
        entity_refs = [EntityReference("a"), EntityReference("b")]
        entity_traits = [TraitsData({"entity"})]
        relationship_traits = TraitsData({"relationship"})
        native_data = 123
        ui_state_1 = UIDelegateState(native_data, entity_refs, entity_traits, relationship_traits)

        ui_state_2 = UIDelegateState(
            native_data, entity_refs, entity_traits, relationship_traits)
        assert ui_state_1 != ui_state_2
        ui_state_2 = UIDelegateState(
            native_data, entity_refs, entity_traits, TraitsData({"different"})
        )
        assert ui_state_1 != ui_state_2
