cmake_minimum_required(VERSION 3.19)
project(mimic_grasping_server)

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

#Static Libs subdirectories
add_subdirectory(${SUBDIR_LIBS_PATH}/SimpleSerialInterface)
set(SIMPLE_SERIAL_INCLUDE_DIR libs/SimpleSerialInterface/include/)

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
        include
        ${SIMPLE_SERIAL_INCLUDE_DIR}
        ${Eigen3_INCLUDE_DIRS}
        ${BOOST_INCLUDE_DIRS}
)

## Declare a C++ library
add_library(mimic_grasping_server
            src/mimic_grasping_server.cpp
            src/tool_firmware_interface.cpp
        )

## Set dependencies of the library
target_link_libraries(mimic_grasping_server
        LINK_PUBLIC simple_serial
        ${BOOST_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${EIGEN_LIBRARIES}
        Boost::thread
        Boost::timer
        jsoncpp
        )

## Define an executable
add_executable(firmware_interface
        examples/firmware_interface.cpp)

## Define the executable dependencies
target_link_libraries(firmware_interface
        mimic_grasping_server
        )

