cmake_minimum_required(VERSION 3.5)
project(fortran_ros2)

enable_language(Fortran)

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rcl REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_runtime_c REQUIRED)

include_directories(include)

add_library(ros2_c_wrapper SHARED
  src/ros2_c_wrapper.c
)

ament_target_dependencies(ros2_c_wrapper
  rcl
  std_msgs
  rosidl_runtime_c
)

add_library(ros2_fortran_module STATIC
  src/ros2_fortran.f90
)

target_link_libraries(ros2_fortran_module
  ros2_c_wrapper
)

file(GLOB FORTRAN_EXECUTABLES 
  src/*.f90
)

set(ALL_FORTRAN_TARGETS "")

foreach(FORTRAN_SRC ${FORTRAN_EXECUTABLES})
  get_filename_component(EXEC_NAME ${FORTRAN_SRC} NAME_WE)
  
  if(NOT ${EXEC_NAME} STREQUAL "ros2_fortran")
    add_executable(${EXEC_NAME}
      ${FORTRAN_SRC}
    )
    
    target_link_libraries(${EXEC_NAME}
      ros2_fortran_module
      ros2_c_wrapper
    )
    
    ament_target_dependencies(${EXEC_NAME}
      rcl
      std_msgs
      rosidl_runtime_c
    )
    
    list(APPEND ALL_FORTRAN_TARGETS ${EXEC_NAME})
  endif()
endforeach()

install(TARGETS ros2_c_wrapper
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)

install(TARGETS ${ALL_FORTRAN_TARGETS}
  DESTINATION lib/${PROJECT_NAME}
)

ament_export_libraries(ros2_c_wrapper)

ament_export_dependencies(
  rcl
  std_msgs
  rosidl_runtime_c
)

ament_package()
