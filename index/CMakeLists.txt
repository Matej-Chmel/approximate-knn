cmake_minimum_required(VERSION 3.0)

project(approximate-knn-index)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(chm)

add_executable(checkDataset apps/checkDataset.cpp)
target_compile_definitions(checkDataset PRIVATE INDEX_PATH="${CMAKE_SOURCE_DIR}")
target_include_directories(
	checkDataset PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/chm"
)
target_link_libraries(checkDataset PUBLIC chm)
