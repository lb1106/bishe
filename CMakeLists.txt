cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(test_pcd_view)

find_package(PCL 1.2 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable (test_pcd_view test_pcd_view.cpp)
add_executable (fileServer fileServer.cpp)
add_executable (fileClient fileClient.cpp)
target_link_libraries (test_pcd_view ${PCL_LIBRARIES})
target_link_libraries (fileClient ${PCL_LIBRARIES})
target_link_libraries (fileServer ${PCL_LIBRARIES})
