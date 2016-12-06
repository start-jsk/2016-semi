cmake_minimum_required(VERSION 2.8)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB Source ${PROJECT_SOURCE_DIR}/src/*.c*)
add_executable(khr_driver ${Source})
target_link_libraries(khr_driver
  ftdi
  usb
)
