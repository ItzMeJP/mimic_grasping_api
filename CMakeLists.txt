cmake_minimum_required(VERSION 3.16)
project(mimic_grasping_api
        VERSION 08.10.2021
        DESCRIPTION
        "Mimic grasping server API to C++")

set(CMAKE_C_COMPILER "gcc-10")
set(CMAKE_CXX_COMPILER "/usr/bin/g++-10")
set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)

get_filename_component(CURRENT_DIR ./ ABSOLUTE)

#set (INSTALL_PLUGIN_SYSTEM_MANAGEMENT_IN_ROOT True)
set(PLUGINS_DIR_FOLDER ${CURRENT_DIR}/plugins)

set(CMAKE_CXX_STANDARD 17)
set(SUBDIR_LIBS_PATH libs)
set(BUILD_PLUGINS False)

#set(EXPORT_PATH_USED_INTO_SUBDIRS #defining a shared CMAKE var.
#        include
#        ${CMAKE_CURRENT_SOURCE_DIR}
#        CACHE INTERNAL "")
#unset(EXPORT_PATH_USED_INTO_SUBDIRS CACHE)

#External packages
find_package(Eigen3 REQUIRED) # To matrix operations
#find_package(Threads REQUIRED) # Multithreading
find_package(Boost REQUIRED COMPONENTS system thread timer) # Multithreading

## Add other libs that does not have an find_package
add_library(simple_serial SHARED IMPORTED GLOBAL)
set_property(TARGET simple_serial PROPERTY IMPORTED_LOCATION "/usr/local/lib/simple_serial/libsimple_serial.so")

add_library(simple_network SHARED IMPORTED GLOBAL)
set_property(TARGET simple_network PROPERTY IMPORTED_LOCATION "/usr/local/lib/simple_network/libsimple_network.so")

add_library(plugin_system_management SHARED IMPORTED GLOBAL)
set_property(TARGET plugin_system_management PROPERTY IMPORTED_LOCATION "/usr/local/lib/plugin_system_management/libplugin_system_management.so")

add_library(transform_manipulation SHARED IMPORTED GLOBAL)
set_property(TARGET transform_manipulation PROPERTY IMPORTED_LOCATION "/usr/local/lib/transform_manipulation/libtransform_manipulation.so")


#Static Libs subdirectories
#add_subdirectory(${SUBDIR_LIBS_PATH}/SimpleSerialInterface)
#set(SIMPLE_SERIAL_INCLUDE_DIR libs/SimpleSerialInterface/include/simple_serial)

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
        include/mimic_grasping_api
        include/plugins
        #${SIMPLE_SERIAL_INCLUDE_DIR}
        #${Eigen3_INCLUDE_DIRS}
        #${BOOST_INCLUDE_DIRS}
)
## ----------------------------------------------------------------------------------------------------------------------------------
## -----------------------------------------------------------  MAIN LIBs -----------------------------------------------------------
## ----------------------------------------------------------------------------------------------------------------------------------


add_library(${PROJECT_NAME} SHARED #Shared is to export it latter
        src/${PROJECT_NAME}/mimic_grasping_server.cpp
        src/${PROJECT_NAME}/tool_firmware_interface.cpp
        src/${PROJECT_NAME}/localization_interface.cpp
        src/${PROJECT_NAME}/dataset_manipulator.cpp
        )

target_link_libraries(${PROJECT_NAME}
        simple_serial
        simple_network
        plugin_system_management
        transform_manipulation
        #${BOOST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        #${EIGEN_LIBRARIES}
        Eigen3::Eigen
        Boost::thread
        Boost::timer
        jsoncpp
        )

target_include_directories(${PROJECT_NAME} PRIVATE .)

# Setting installation properties
set_target_properties(${PROJECT_NAME} PROPERTIES
        VERSION ${PROJECT_VERSION}
        #PUBLIC_HEADER "include/mimic_grasping_server/mimic_grasping_server.h"
        #PRIVATE_HEADER "include/mimic_grasping_server/tool_firmware_interface.h"
        PUBLIC_HEADER "include/${PROJECT_NAME}/mimic_grasping_server.h;include/${PROJECT_NAME}/localization_base.h;include/${PROJECT_NAME}/dataset_manipulator.h;include/${PROJECT_NAME}/tool_firmware_interface.h;include/${PROJECT_NAME}/localization_interface.h"
        )

configure_file(install_configurations/${PROJECT_NAME}.pc.in ${PROJECT_NAME}.pc @ONLY)

# Installing libs
install(TARGETS ${PROJECT_NAME}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

## ----------------------------------------------------------------------------------------------------------------------------------
## -----------------------------------------------------------  Plugins LIBs --------------------------------------------------------
## ----------------------------------------------------------------------------------------------------------------------------------

if (${BUILD_PLUGINS})
    MESSAGE("The plugins will be builded.")

    # Declaring
    add_library(foo SHARED #Shared is to export it latter
            src/plugins/foo.cpp
            )

    add_library(object_localization_ros SHARED #Shared is to export it latter
            src/plugins/obj_localization_ros.cpp
            )

    target_link_libraries(foo
            ${PROJECT_NAME}
            )

    target_link_libraries(object_localization_ros
            ${PROJECT_NAME}
            Boost::thread
            )

    target_include_directories(foo PRIVATE .)

    set_target_properties(foo PROPERTIES
            #VERSION ${PROJECT_VERSION}
            PUBLIC_HEADER "include/plugins/foo.h"
            )
    target_include_directories(object_localization_ros PRIVATE .)

    set_target_properties(object_localization_ros PROPERTIES
            #VERSION ${PROJECT_VERSION}
            PUBLIC_HEADER "include/plugins/obj_localization_ros.h"
            )

    configure_file(install_configurations/foo.pc.in foo.pc @ONLY)
    configure_file(install_configurations/obj_localization_ros.pc.in obj_localization_ros.pc @ONLY)

    install(TARGETS foo
            LIBRARY DESTINATION ${PLUGINS_DIR_FOLDER}
            PUBLIC_HEADER DESTINATION ${CMAKE_BINARY_DIR}/public_headers/plugins)

    install(TARGETS object_localization_ros
            LIBRARY DESTINATION ${PLUGINS_DIR_FOLDER}
            PUBLIC_HEADER DESTINATION ${CMAKE_BINARY_DIR}/public_headers/plugins)

    install(FILES ${CMAKE_BINARY_DIR}/foo.pc
            DESTINATION ${CMAKE_BINARY_DIR}/pkgconfig)

    install(FILES ${CMAKE_BINARY_DIR}/obj_localization_ros.pc
            DESTINATION ${CMAKE_BINARY_DIR}/pkgconfig)

else ()
    MESSAGE("The plugins will not be builded.")
endif ()

## ----------------------------------------------------------------------------------------------------------------------------------
## --------------------------------------------------------  Executables ------------------------------------------------------------
## ----------------------------------------------------------------------------------------------------------------------------------
## Define an executable
add_executable(run
        src/examples/run.cpp)

## Define the executable dependencies
target_link_libraries(run
        ${PROJECT_NAME}
        )
