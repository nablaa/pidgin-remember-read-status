include_directories(
    ../include
    ${PIDGIN_INCLUDE_DIRS}
    ${LIBPURPLE_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    )

add_library(conversation_history conversation_history.c)
add_library(remember_read_status SHARED remember_read_status.c conversation_history)

target_link_libraries(remember_read_status
    ${PIDGIN_LIBRARIES}
    ${LIBPURPLE_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${GTK_LIBRARIES}
    )

set(PLUGIN_INSTALL_DIR "~/.purple/plugins/")
install(TARGETS remember_read_status DESTINATION "${PLUGIN_INSTALL_DIR}")
