include (plugin)

set(CURRENT_PLUGIN_DEPS "")

if (EXISTS ${CMAKE_SOURCE_DIR}/ObjectTypes/PolyhedralMesh)
    add_definitions(-DENABLE_OPENVOLUMEMESH_SUPPORT)
    add_definitions(-DENABLE_OPENVOLUMEMESH_POLYHEDRAL_SUPPORT)

    list(APPEND CURRENT_PLUGIN_DEPS OpenVolumeMesh)
endif()

list(REMOVE_DUPLICATES CURRENT_PLUGIN_DEPS)

openflipper_plugin (
    DEPS ${CURRENT_PLUGIN_DEPS}
)
