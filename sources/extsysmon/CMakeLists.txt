set(SUBPROJECT plasma_dataengine_extsysmon)
message(STATUS "Subproject ${SUBPROJECT}")
add_definitions(-DTRANSLATION_DOMAIN=\"plasma_applet_org.kde.plasma.awesomewidget\")

include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_LIBRARY}/
        ${CMAKE_CURRENT_SOURCE_DIR}/../${PROJECT_MONITORSOURCES}/
        ${PROJECT_TRDPARTY_DIR}
        ${Qt_INCLUDE}
        ${Kf5_INCLUDE}
)

file(GLOB SUBPROJECT_DESKTOP_IN *.desktop)
file(RELATIVE_PATH SUBPROJECT_DESKTOP ${CMAKE_SOURCE_DIR} ${SUBPROJECT_DESKTOP_IN})
file(GLOB SUBPROJECT_SOURCE *.cpp ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB SUBPROJECT_HEADER *.h)
file(GLOB SUBPROJECT_CONF *.conf)

# prepare
configure_file(${SUBPROJECT_DESKTOP_IN} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP})

# make
add_library(${SUBPROJECT} MODULE ${SUBPROJECT_SOURCE} ${SUBPROJECT_HEADER})
target_link_libraries(${SUBPROJECT} 
        ${PROJECT_LIBRARY} ${PROJECT_MONITORSOURCES} 
        ${Qt_LIBRARIES} ${Kf5_LIBRARIES})
kcoreaddons_desktop_to_json(${SUBPROJECT} ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP}
                            SERVICE_TYPES plasma-dataengine.desktop)

# install
install(TARGETS ${SUBPROJECT} DESTINATION ${PLUGIN_INSTALL_DIR}/plasma/dataengine)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SUBPROJECT_DESKTOP} DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES ${SUBPROJECT_CONF} DESTINATION ${CONFIG_INSTALL_DIR})
