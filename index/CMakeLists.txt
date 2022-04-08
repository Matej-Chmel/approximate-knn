file(GLOB headers chm/*.hpp chm_tools/*.hpp)
file(GLOB sources chm/*.cpp chm_tools/*.cpp)
source_group("Headers" FILES ${headers})
source_group("Sources" FILES ${sources})

add_library(chmLib ${headers} ${sources})
target_compile_definitions(chmLib PRIVATE AVX_CAPABLE SIMD_CAPABLE SSE_CAPABLE)
target_include_directories(chmLib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
