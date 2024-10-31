#  SPDX-License-Identifier: Apache-2.0
#  Copyright 2024 The Foundry Visionmongers Ltd
from unittest import mock

import pytest

from openassetio.ui.managerApi import UIDelegateInterface


class MockUIDelegateInterface(UIDelegateInterface):

    def __init__(self):
        UIDelegateInterface.__init__(self)
        self.mock = mock.create_autospec(UIDelegateInterface)

    def identifier(self):
        return self.mock.identifier()

    def displayName(self):
        return self.mock.displayName()

    def info(self):
        return self.mock.info()

    def settings(self, hostSession):
        return self.mock.settings(hostSession)

    def initialize(self, uiDelegateSettings, hostSession):
        return self.mock.initialize(uiDelegateSettings, hostSession)

    def close(self, hostSession):
        return self.mock.close(hostSession)

    def flushCaches(self, hostSession):
        return self.mock.flushCaches(hostSession)

    def uiPolicy(self, uiTraits, uiPolicyAccess, context, hostSession):
        return self.mock.uiPolicy(uiTraits, uiPolicyAccess, context, hostSession)

    def populateUI(self, container, uiTraits, entityTraits, *args):
        return self.mock.populateUI(container, uiTraits, entityTraits, *args)


@pytest.fixture
def mock_ui_delegate_interface():
    return MockUIDelegateInterface()
