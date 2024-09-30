# SPDX-License-Identifier: Apache-2.0
# Copyright 2024 The Foundry Visionmongers Ltd

#-----------------------------------------------------------------------
# Test Plugins

# Create a test plugin with a particular path and id suffix.
function(openassetio_test_pluginSystem_generate_plugin path_suffix id_suffix src_file)

    #-------------------------------------------------------------------
    # Create plugin

    add_library(openassetio-ui-pluginSystem-test-${path_suffix} MODULE)
    openassetio_set_default_target_properties(openassetio-ui-pluginSystem-test-${path_suffix})
    openassetio_simplify_lib_name(openassetio-ui-pluginSystem-test-${path_suffix} ${path_suffix})
    # Add to the set of installable targets.
    install(
        TARGETS openassetio-ui-pluginSystem-test-${path_suffix}
        EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS
        DESTINATION ${OPENASSETIO_TEST_CPP_PLUGINS_SUBDIR}/ui/${path_suffix}
    )

    #-------------------------------------------------------------------
    # Plugin identification

    target_compile_definitions(
        openassetio-ui-pluginSystem-test-${path_suffix}
        PRIVATE
        # Suffix for plugin path
        OPENASSETIO_UI_PLUGINSYSTEM_TEST_PLUGIN_PATH_SUFFIX="${path_suffix}"
        # Suffix for plugin identifier
        OPENASSETIO_UI_PLUGINSYSTEM_TEST_PLUGIN_ID_SUFFIX="${id_suffix}"
    )

    #-------------------------------------------------------------------
    # Target dependencies

    target_sources(
        openassetio-ui-pluginSystem-test-${path_suffix}
        PRIVATE
        ${src_file}
    )

    target_link_libraries(
        openassetio-ui-pluginSystem-test-${path_suffix}
        PRIVATE
        # Core library
        openassetio-ui
        openassetio-core
    )

    target_include_directories(
        openassetio-ui-pluginSystem-test-${path_suffix}
        PRIVATE
        # For export header
        ${CMAKE_CURRENT_BINARY_DIR}/${path_suffix}/include
    )

    #-------------------------------------------------------------------
    # API export header

    include(GenerateExportHeader)
    generate_export_header(
        openassetio-ui-pluginSystem-test-${path_suffix}
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/${path_suffix}/include/export.h
        EXPORT_MACRO_NAME OPENASSETIO_UI_PLUGINSYSTEM_TEST_EXPORT
    )

endfunction()

# Plugins for testing the CppPluginSystem
#
# Path/ID chosen to mirror Python plugin system test plugins.

if (OPENASSETIO_ENABLE_PYTHON)
    # Plugin for testing the Python GIL is released
    openassetio_test_pluginSystem_generate_plugin(
        python-gil-check-ui python-gil-check-ui pythonGilCheckUIPlugin.cpp)
    target_link_libraries(
        openassetio-ui-pluginSystem-test-python-gil-check-ui
        PRIVATE
        $<BUILD_INTERFACE:pybind11::module>
    )
endif ()

# Plugins for testing the CppPluginSystemUIDelegateImplementationFactory.
openassetio_test_pluginSystem_generate_plugin(uiA pluginA uiPlugin.cpp)
openassetio_test_pluginSystem_generate_plugin(uiB pluginB uiPlugin.cpp)
