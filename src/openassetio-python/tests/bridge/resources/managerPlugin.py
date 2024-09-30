"""
Provides a test PythonPluginSystemManagerPlugin implemented within a
single file module.
"""

from openassetio.pluginSystem.PythonPluginSystemManagerPlugin import (
    PythonPluginSystemManagerPlugin,
)


class Plugin(PythonPluginSystemManagerPlugin):
    # pylint: disable=missing-class-docstring

    @classmethod
    def identifier(cls):
        return "org.openassetio.test.pluginSystem.resources.managerPlugin"


# pylint: disable=invalid-name
openassetioPlugin = Plugin
