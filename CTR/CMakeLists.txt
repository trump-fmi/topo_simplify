cmake_minimum_required(VERSION 3.1)
project(Topological_Simplification)

find_package(CGAL)
add_executable(topo_simplify topo_simplify.cpp)
target_link_libraries(topo_simplify CGAL::CGAL)

