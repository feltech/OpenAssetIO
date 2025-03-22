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
"""
Module hoisting the UIDelegateRequestInterface and
UIDelegateStateInterface abstract base classes, as well as providing
default convenience Python implementations of them, i.e.
UIDelegateRequest and UIDelegateState, respectively.
"""
from .. import _openassetio  # pylint: disable=no-name-in-module

UIDelegateRequestInterface = _openassetio.ui.UIDelegateRequestInterface
UIDelegateStateInterface = _openassetio.ui.UIDelegateStateInterface


class UIDelegateRequest(UIDelegateRequestInterface):
    """
    Default convenience implementation of
    @fqref{ui.UIDelegateRequestInterface} "UIDelegateStateInterface" for
    use in Python host applications.

    Adds member variables and setter methods.
    """

    def __init__(
        self,
        nativeData=None,
        entityReferences=None,
        entityTraitsDatas=None,
        stateChangedCallback=None,
    ):
        """
        Constructs a new instance.

        Arguments that are left as `None` will be converted to empty
        lists where appropriate.

        @param nativeData Optional native data object to initialize
        with.

        @param entityReferences Optional list of entity references to
        initialize with.

        @param entityTraitsDatas Optional list of traits and their
        properties to initialize with.

        @param stateChangedCallback Optional callback function to handle
        state changes.
        """
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
        """
        Returns the held native data object.

        @see @fqref{ui.UIDelegateRequestInterface.nativeData}
        "nativeData"
        """
        return self.__nativeData

    def entityReferences(self):
        """
        Returns the held entity reference list.

        @see @fqref{ui.UIDelegateRequestInterface.entityReferences}
        "entityReferences"
        """
        return self.__entityReferences

    def entityTraitsDatas(self):
        """
        Returns the held traits data list.

        @see @fqref{ui.UIDelegateRequestInterface.entityTraitsDatas}
        "entityTraitsDatas"
        """
        return self.__entityTraitsDatas

    def stateChangedCallback(self):
        """
        Returns the held callback.

        @see @fqref{ui.UIDelegateRequestInterface.stateChangedCallback}
        "stateChangedCallback"
        """
        return self.__stateChangedCallback

    def setNativeData(self, nativeData):
        """
        Sets the held native data object.

        @param nativeData The native data object to set
        """
        self.__nativeData = nativeData

    def setEntityReferences(self, entityReferences):
        """
        Sets the list of entity references.

        @param entityReferences List of entity references to set
        """
        self.__entityReferences = entityReferences

    def setEntityTraitsDatas(self, entityTraitsDatas):
        """
        Sets the list of entity traits data.

        @param entityTraitsDatas List of traits data to set.
        """
        self.__entityTraitsDatas = entityTraitsDatas

    def setStateChangedCallback(self, stateChangedCallback):
        """
        Sets the callback function for handling state changes.

        @param stateChangedCallback Callback function to set
        """
        self.__stateChangedCallback = stateChangedCallback


class UIDelegateState(UIDelegateStateInterface):
    """
    Default convenience implementation of
    @fqref{ui.UIDelegateStateInterface} "UIDelegateStateInterface" for
    use in Python UI delegate plugins.

    Adds member variables and setter methods.
    """

    def __init__(
        self,
        nativeData=None,
        entityReferences=None,
        entityTraitsDatas=None,
        updateRequestCallback=None,
    ):
        """
        Constructs a new instance.

        Arguments that are left as `None` will be converted to empty
        lists where appropriate.

        @param nativeData Optional native data object to initialize
        with.

        @param entityReferences Optional list of entity references to
        initialize with

        @param entityTraitsDatas Optional list of traits and their
        properties to initialize with.

        @param updateRequestCallback Optional callback function to
        handle request updates.
        """
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
        """
        Returns the held native data object.

        @see  @fqref{ui.UIDelegateStateInterface.nativeData}
        "nativeData".
        """
        return self.__nativeData

    def entityReferences(self):
        """
        Returns the held entity reference list.

        @see @fqref{ui.UIDelegateStateInterface.entityReferences}
        "entityReferences".
        """
        return self.__entityReferences

    def entityTraitsDatas(self):
        """
        Returns the held traits data list.

        @see @fqref{ui.UIDelegateStateInterface.entityTraitsDatas}
        "entityTraitsDatas".
        """
        return self.__entityTraitsDatas

    def updateRequestCallback(self):
        """
        Returns the held callback.

        @see @fqref{ui.UIDelegateStateInterface.updateRequestCallback}
        "updateRequestCallback".
        """
        return self.__updateRequestCallback

    def setNativeData(self, nativeData):
        """
        Sets the held native data object.

        @param nativeData The native object to set.
        """
        self.__nativeData = nativeData

    def setEntityReferences(self, entityReferences):
        """
        Sets the list of entity references.

        @param entityReferences List of entity references to set.
        """
        self.__entityReferences = entityReferences

    def setEntityTraitsDatas(self, entityTraitsDatas):
        """
        Sets the list of entity traits data.

        @param entityTraitsDatas List of trait data to set.
        """
        self.__entityTraitsDatas = entityTraitsDatas

    def setUpdateRequestCallback(self, updateRequestCallback):
        """
        Sets the callback function for handling request updates.

        @param updateRequestCallback Callback function to set.
        """
        self.__updateRequestCallback = updateRequestCallback
