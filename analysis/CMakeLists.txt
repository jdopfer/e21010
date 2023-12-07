cmake_minimum_required(VERSION 3.18)
project(analysis)

if(CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
  find_library(_config++_path config++)
  get_filename_component(_config++_path ${_config++_path} DIRECTORY)
  link_directories(${_config++_path})
endif()
link_libraries(AUSA::AUSA config++ telescope::telescope GSL::gsl)

file(GLOB HEADERS CONFIGURE_DEPENDS "include/*.h")
add_executable(bp bp.cpp ${HEADERS})
target_include_directories(bp PRIVATE include)