cmake_minimum_required(VERSION 3.1)
project(X_Free_SubSegments)

find_package(CGAL)
add_executable(make_x_free make_x_free.cpp)
target_link_libraries(make_x_free CGAL::CGAL)

