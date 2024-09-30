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
from .. import _openassetio

constants = _openassetio.ui.constants
UIDelegateStateInterface = _openassetio.ui.UIDelegateStateInterface
UIDelegateRequestInterface = _openassetio.ui.UIDelegateRequestInterface


class UIDelegateState(UIDelegateStateInterface):
    """
    Default convenience implementation of UIDelegateStateInterface for
    use in Python UI delegate plugins.

    Adds member variables and setter methods.
    """
    # pylint: disable=all
    # TODO(DF): enable linting.
    def __init__(
        self,
        nativeData=None,
        entityReferences=None,
        entityTraitsDatas=None,
        updateRequestCallback=None,
    ):
        UIDelegateStateInterface.__init__(self)
        self.__nativeData = nativeData
        if entityReferences is None:
            entityReferences = []
        self.__entityReferences = entityReferences
        if entityTraitsDatas is None:
            entityTraitsDatas = []
        self.__entityTraitsDatas = entityTraitsDatas
        self.__updateRequestCallback = updateRequestCallback

    def nativeData(self):
        return self.__nativeData

    def entityReferences(self):
        return self.__entityReferences

    def entityTraitsDatas(self):
        return self.__entityTraitsDatas

    def updateRequestCallback(self):
        return self.__updateRequestCallback

    def setNativeData(self, nativeData):
        self.__nativeData = nativeData

    def setEntityReferences(self, entityReferences):
        self.__entityReferences = entityReferences

    def setEntityTraitsDatas(self, entityTraitsDatas):
        self.__entityTraitsDatas = entityTraitsDatas

    def setUpdateRequestCallback(self, updateRequestCallback):
        self.__updateRequestCallback = updateRequestCallback


class UIDelegateRequest(UIDelegateRequestInterface):
    """
    Default convenience implementation of UIDelegateRequestInterface for
    use in Python host applications.

    Adds member variables and setter methods.
    """
    # pylint: disable=all
    # TODO(DF): enable linting.
    def __init__(
        self,
        nativeData=None,
        entityReferences=None,
        entityTraitsDatas=None,
        stateChangedCallback=None,
    ):
        UIDelegateRequestInterface.__init__(self)
        self.__nativeData = nativeData
        if entityReferences is None:
            entityReferences = []
        self.__entityReferences = entityReferences
        if entityTraitsDatas is None:
            entityTraitsDatas = []
        self.__entityTraitsDatas = entityTraitsDatas
        self.__stateChangedCallback = stateChangedCallback

    def nativeData(self):
        return self.__nativeData

    def entityReferences(self):
        return self.__entityReferences

    def entityTraitsDatas(self):
        return self.__entityTraitsDatas

    def stateChangedCallback(self):
        return self.__stateChangedCallback

    def setNativeData(self, nativeData):
        self.__nativeData = nativeData

    def setEntityReferences(self, entityReferences):
        self.__entityReferences = entityReferences

    def setEntityTraitsDatas(self, entityTraitsDatas):
        self.__entityTraitsDatas = entityTraitsDatas

    def setStateChangedCallback(self, stateChangedCallback):
        self.__stateChangedCallback = stateChangedCallback
