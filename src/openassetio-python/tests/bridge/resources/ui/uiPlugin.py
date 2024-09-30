"""
Provides a test PythonPluginSystemUIDelegatePlugin implemented within a
single file module.
"""

from openassetio.ui.pluginSystem.PythonPluginSystemUIDelegatePlugin import (
    PythonPluginSystemUIDelegatePlugin,
)


class Plugin(PythonPluginSystemUIDelegatePlugin):
    # pylint: disable=missing-class-docstring

    @classmethod
    def identifier(cls):
        return "org.openassetio.test.pluginSystem.resources.uiPlugin"


# pylint: disable=invalid-name
openassetioUIPlugin = Plugin
