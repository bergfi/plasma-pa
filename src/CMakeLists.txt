include_directories(${PULSEAUDIO_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR}     ${GCONF_INCLUDE_DIRS}
    ${GOBJECT_INCLUDE_DIRS})

set(cpp_SRCS
    card.cpp
    client.cpp
    context.cpp
    device.cpp
    maps.cpp
    operation.cpp
    port.cpp
    profile.cpp
    pulseaudio.cpp
    pulseobject.cpp
    sink.cpp
    sinkinput.cpp
    modulemanager.cpp
    gconfitem.cpp
    source.cpp
    sourceoutput.cpp
    stream.cpp
    volumeobject.cpp
    debug.cpp
    server.cpp
    streamrestore.cpp
    module.cpp
    qml/globalactioncollection.cpp
    qml/plugin.cpp
    qml/volumeosd.cpp
    qml/volumefeedback.cpp
)

set(qml_SRCS
    qml/qmldir
    qml/PulseObjectFilterModel.qml
)

set_property(SOURCE qml/dbus/osdService.xml APPEND PROPERTY CLASSNAME OsdServiceInterface)
qt5_add_dbus_interface(dbus_SRCS qml/dbus/osdService.xml osdservice)

add_library(plasma-volume-declarative SHARED ${dbus_SRCS} ${cpp_SRCS} ${qml_SRCS})
target_link_libraries(plasma-volume-declarative
    Qt5::Core
    Qt5::Gui
    Qt5::DBus
    Qt5::Quick
    KF5::GlobalAccel
    ${PULSEAUDIO_LIBRARY}
    ${PULSEAUDIO_MAINLOOP_LIBRARY}
    ${GCONF_LDFLAGS}
    ${GOBJECT_LDFLAGS}
    ${CANBERRA_LIBRARIES}
)

set(PRIVATE_QML_INSTALL_DIR ${QML_INSTALL_DIR}/org/kde/plasma/private/volume)
install(TARGETS plasma-volume-declarative DESTINATION ${PRIVATE_QML_INSTALL_DIR})
install(FILES ${qml_SRCS} DESTINATION ${PRIVATE_QML_INSTALL_DIR})

add_subdirectory(kcm)
