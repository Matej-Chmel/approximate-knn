cmake_minimum_required(VERSION 3.0)

project(approximate-knn-index)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(run apps/run.cpp)
add_subdirectory(chm)

target_link_libraries(run PUBLIC chm)
target_include_directories(
	run PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/chm"
)
