cmake_minimum_required(VERSION 3.0)

project(approximate-knn-index)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(index)

add_executable(checkDataset executables/checkDataset.cpp)
target_compile_definitions(checkDataset PRIVATE REPO_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(
	checkDataset PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/index"
)
target_link_libraries(checkDataset PUBLIC chmLib)

add_executable(recallTable executables/recallTable.cpp)
target_compile_definitions(recallTable PRIVATE REPO_DIR="${CMAKE_SOURCE_DIR}")
target_include_directories(
	recallTable PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/index"
)
target_link_libraries(recallTable PUBLIC chmLib)
