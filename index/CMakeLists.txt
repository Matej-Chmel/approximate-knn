file(GLOB sources chm/*.cpp chm_tools/*.cpp)
add_library(chmLib ${sources})
target_include_directories(chmLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
