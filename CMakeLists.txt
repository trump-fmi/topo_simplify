cmake_minimum_required(VERSION 3.1)
project(TopologicalSimplification)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 14)
find_package(CGAL)
add_executable(topo_simplify topo_simplify.cpp)
target_link_libraries(topo_simplify CGAL::CGAL)

