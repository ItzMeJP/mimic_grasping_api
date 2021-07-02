cmake_minimum_required(VERSION 3.16)
project(mimic_grasping_server
        VERSION 26.06.2021
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
set_property(TARGET simple_serial PROPERTY IMPORTED_LOCATION "/usr/local/lib/libsimple_serial.so")

add_library(plugin_system_management SHARED IMPORTED GLOBAL)
set_property(TARGET plugin_system_management PROPERTY IMPORTED_LOCATION "/usr/local/lib/libplugin_system_management.so")


#Static Libs subdirectories
#add_subdirectory(${SUBDIR_LIBS_PATH}/SimpleSerialInterface)
#set(SIMPLE_SERIAL_INCLUDE_DIR libs/SimpleSerialInterface/include/simple_serial)

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
        include/mimic_grasping_server
        include/plugins
        #${SIMPLE_SERIAL_INCLUDE_DIR}
        #${Eigen3_INCLUDE_DIRS}
        #${BOOST_INCLUDE_DIRS}
)

## Declare a C++ library
add_library(mimic_grasping_server SHARED #Shared is to export it latter
            src/mimic_grasping_server.cpp
            src/tool_firmware_interface.cpp
        )

add_library(pose SHARED #Shared is to export it latter
        src/pose.cpp
        )

add_library(foo SHARED #Shared is to export it latter
        src/plugins/foo.cpp
        )

## Set dependencies of the library
target_link_libraries(pose
        Eigen3::Eigen
        )

target_link_libraries(mimic_grasping_server
        simple_serial
        plugin_system_management
        pose
        #${BOOST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        #${EIGEN_LIBRARIES}
        Eigen3::Eigen
        Boost::thread
        Boost::timer
        jsoncpp
        )

target_link_libraries(foo
        pose
        )

# To export the lib and install it
set_target_properties(mimic_grasping_server PROPERTIES
        VERSION ${PROJECT_VERSION}
        #PUBLIC_HEADER "include/mimic_grasping_server/mimic_grasping_server.h"
        #PRIVATE_HEADER "include/mimic_grasping_server/tool_firmware_interface.h"
        PUBLIC_HEADER "include/mimic_grasping_server/mimic_grasping_server.h;include/mimic_grasping_server/tool_firmware_interface.h" #if I want to let both visible
        )

set_target_properties(foo PROPERTIES
        #VERSION ${PROJECT_VERSION}
        PUBLIC_HEADER "include/plugins/foo.h"
        )

target_include_directories(mimic_grasping_server PRIVATE .)

configure_file(ConfigFiles/mimic_grasping_server.pc.in mimic_grasping_server.pc @ONLY)
configure_file(ConfigFiles/foo.pc.in foo.pc @ONLY)

install(TARGETS mimic_grasping_server
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/mimic_grasping_server)

install(TARGETS foo
        LIBRARY DESTINATION ${PLUGINS_DIR_FOLDER}
        PUBLIC_HEADER DESTINATION ${CMAKE_BINARY_DIR}/public_headers/plugins)

install(FILES ${CMAKE_BINARY_DIR}/mimic_grasping_server.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

install(FILES ${CMAKE_BINARY_DIR}/foo.pc
        DESTINATION ${CMAKE_BINARY_DIR}/pkgconfig)

## Define an executable
add_executable(firmware_interface
        examples/firmware_interface.cpp)

## Define the executable dependencies
target_link_libraries(firmware_interface
        mimic_grasping_server
        )
