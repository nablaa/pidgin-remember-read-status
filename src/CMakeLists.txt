include_directories(
    ${PIDGIN_INCLUDE_DIRS}
    ${LIBPURPLE_INCLUDE_DIRS}
    ${GLIB2_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    )

add_library(conv_memory SHARED conv_memory.c)

target_link_libraries(conv_memory
    ${PIDGIN_LIBRARIES}
    ${LIBPURPLE_LIBRARIES}
    ${GLIB2_LIBRARIES}
    ${GTK_LIBRARIES}
    )

set(PLUGIN_INSTALL_DIR "~/.purple/plugins/")
install(TARGETS conv_memory DESTINATION "${PLUGIN_INSTALL_DIR}")
