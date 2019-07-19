cmake_minimum_required(VERSION 3.1)
project(XFREE)

find_package(CGAL)
add_executable(xfree xfree.cpp)
target_link_libraries(xfree CGAL::CGAL)

