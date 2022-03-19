cmake_minimum_required(VERSION 3.0)

project(approximate-knn-index)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(index)

add_executable(checkDataset index/apps/checkDataset.cpp)
target_compile_definitions(checkDataset PRIVATE INDEX_PATH="${CMAKE_SOURCE_DIR}/index")
target_include_directories(
	checkDataset PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/index"
)
target_link_libraries(checkDataset PUBLIC chmLib)

add_executable(recallTable index/apps/recallTable.cpp)
target_compile_definitions(recallTable PRIVATE INDEX_PATH="${CMAKE_SOURCE_DIR}/index")
target_include_directories(
	recallTable PUBLIC
	"${PROJECT_BINARY_DIR}"
	"${PROJECT_SOURCE_DIR}/index"
)
target_link_libraries(recallTable PUBLIC chmLib)
