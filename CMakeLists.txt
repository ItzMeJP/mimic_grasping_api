cmake_minimum_required(VERSION 3.16)
project(mimic_grasping_server
        VERSION 22.06.2021
        DESCRIPTION
        "Mimic grasping server API to C++")

include(GNUInstallDirs)

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

#Static Libs subdirectories
#add_subdirectory(${SUBDIR_LIBS_PATH}/SimpleSerialInterface)
#set(SIMPLE_SERIAL_INCLUDE_DIR libs/SimpleSerialInterface/include/simple_serial)

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
        include/mimic_grasping_server
        #${SIMPLE_SERIAL_INCLUDE_DIR}
        ${Eigen3_INCLUDE_DIRS}
        ${BOOST_INCLUDE_DIRS}
)

## Declare a C++ library
add_library(mimic_grasping_server SHARED #Shared is to export it latter
            src/mimic_grasping_server.cpp
            src/tool_firmware_interface.cpp
        )

## Set dependencies of the library
target_link_libraries(mimic_grasping_server
        simple_serial
        ${BOOST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${EIGEN_LIBRARIES}
        Boost::thread
        Boost::timer
        jsoncpp
        )

# To export the lib and install it
set_target_properties(mimic_grasping_server PROPERTIES
        VERSION ${PROJECT_VERSION}
        #PUBLIC_HEADER "include/mimic_grasping_server/mimic_grasping_server.h"
        #PRIVATE_HEADER "include/mimic_grasping_server/tool_firmware_interface.h"
        PUBLIC_HEADER "include/mimic_grasping_server/mimic_grasping_server.h;include/mimic_grasping_server/tool_firmware_interface.h" #if I want to let both visible
        )

target_include_directories(mimic_grasping_server PRIVATE .)

configure_file(mimic_grasping_server.pc.in mimic_grasping_server.pc @ONLY)

install(TARGETS mimic_grasping_server
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/mimic_grasping_server.pc
        DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)

## Define an executable
add_executable(firmware_interface
        examples/firmware_interface.cpp)

## Define the executable dependencies
target_link_libraries(firmware_interface
        mimic_grasping_server
        )
