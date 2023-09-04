#
#   Copyright 2013-2021 The Foundry Visionmongers Ltd
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
@namespace openassetio.exceptions
Defines exceptions used in the OpenAssetIO codebase.

@todo [tc] Should these all live here, or in their respective homes
eg: pluginSystem, ManagerInterface, etc.
"""


class OpenAssetIOException(RuntimeError):
    """
    The OpenAssetIOException class should be used for all exceptions raise by
    Managers and any API-related exceptions raised in a Host. These
    exceptions are guaranteed to properly bridge across the C plugin
    divide...

    @unstable
    """



class InputValidationError(OpenAssetIOException):
    """
    Thrown whenever input to a public API method fails validation.

    E.g. pageSize, preflight/register list sizes
    """

    def __init__(self, funcName, message="Invalid Entity Reference", entityReference=None):
        super(InputValidationError, self).__init__(message, entityReference)



class WholeBatchError(OpenAssetIOException):
    """
    Indicate that the whole batch has failed cleanly and no state has
    changed.

    @unstable
    """


class ConfigurationError(OpenAssetIOException):
    """
    E.g. defaultManagerForInterface

    Replaces PluginError
    """
    pass


class NotImplementedByManagerError(OpenAssetIOException):
    pass
