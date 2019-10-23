#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <list>
#include <vector>

struct DirectedIndex {
  size_t index;
  bool fwd;

  bool operator==(const DirectedIndex other) const {
    return index == other.index && fwd == other.fwd;
  }
};

typedef CGAL::Exact_predicates_exact_constructions_kernel Kernel;
typedef CGAL::Arr_segment_traits_2<> Geom_traits_2;
typedef Geom_traits_2::Point_2 Point_2;
typedef Geom_traits_2::Segment_2 Segment_2;
typedef Geom_traits_2::Curve_2 Polyline_2;
typedef CGAL::Arr_extended_dcel<Geom_traits_2, bool, DirectedIndex, int>
    Dcel; // Vertex, HE, Face
typedef CGAL::Arrangement_2<Geom_traits_2, Dcel> Arrangement_2;

struct Instance {
  std::vector<Point_2> points;
  std::vector<std::vector<Point_2>> curves;
};

struct XFreeInstance {
  std::vector<Point_2> points;
  std::vector<std::vector<Point_2>> subcurves;
  std::vector<std::vector<DirectedIndex>> curve_parts;
};

void mark_non_deg2(Arrangement_2 &arr) {
  for (auto &vh : arr.vertex_handles()) {
    if (vh->degree() != 2)
      vh->set_data(true);
  }
}

std::vector<Arrangement_2::Halfedge_handle> mark_subcurves(Arrangement_2 &arr) {
  std::vector<Arrangement_2::Halfedge_handle> result;
  for (auto heit : arr.halfedge_handles())
    heit->set_data({0, false});
  size_t i = 0;
  for (auto heit : arr.halfedge_handles()) {
    if (heit->data().fwd || heit->twin()->data().fwd)
      continue; // already visited
    auto he = heit;
    while (!he->source()->data())
      he = he->prev();
    result.push_back(he);
    do {
      he->set_data({i, true});
      he->twin()->set_data({i, false});
      he = he->next();
    } while (!he->source()->data());
    ++i;
  }
  return result;
}

template <typename It>
std::vector<Arrangement_2::Halfedge>
trace_points(Arrangement_2 &arr, It points_begin, It points_end) {
  std::vector<Arrangement_2::Halfedge> result;
  It pit = points_begin;
  auto vh = CGAL::insert_point(arr, *pit);
  // find all points
  while (++pit != points_end) {
    // collect all halfedges to the next point
    while (vh->point() != *pit) {
      auto heit = vh->incident_halfedges();
      // find correct halfedge
      while (!Kernel::Segment_2(vh->point(), *pit)
                  .has_on(heit->source()->point())) {
        ++heit;
      }
      vh = heit->source();
      result.push_back(*heit);
    }
  }
  return result;
}

Instance read_instance(std::istream &stream) {
  std::string line;

  // points
  std::vector<Point_2> points;
  {
    std::getline(std::cin, line);
    std::stringstream linestream(line);
    auto points_begin = std::istream_iterator<Point_2>(linestream);
    auto points_end = std::istream_iterator<Point_2>();
    points = {points_begin, points_end};
  }

  // constraints
  std::vector<std::vector<Point_2>> curves;
  while (std::getline(std::cin, line)) {
    std::stringstream linestream(line);
    auto points_begin = std::istream_iterator<Point_2>(linestream);
    auto points_end = std::istream_iterator<Point_2>();
    curves.emplace_back(points_begin, points_end);
  }
   
  return {points, curves};
}

int main(int argc, char** argv) {

  Arrangement_2 arr;
  
  Instance instance = read_instance(std::cin);

  auto &curves = instance.curves;
  auto &points = instance.points;


  for (auto &c : curves) {
    CGAL::insert_point(arr, c.front());
    CGAL::insert_point(arr, c.back());
  }
  for (auto p : points)
    CGAL::insert_point(arr, p);
  mark_non_deg2(arr);

  std::vector<Segment_2> segments;
  for (auto &c : curves) {
    for (int i = 1; i < c.size(); ++i) {
      segments.push_back(Segment_2(c[i - 1], c[i]));
    }
  }
  CGAL::insert(arr, segments.begin(), segments.end());
  mark_non_deg2(arr);

  auto parts = mark_subcurves(arr);
  
  std::copy(points.begin(), points.end(), std::ostream_iterator<Point_2>(std::cout, " "));
  std::cout << std::endl;

  std::cout << parts.size() << std::endl;
  for (auto he : parts) {
    do {
      std::cout << he->source()->point() << " ";
      he = he->next();
    } while (!he->source()->data());
    std::cout << he->source()->point() << " ";
    std::cout << std::endl;
  }

  std::cout << curves.size() << std::endl;
  for (auto &c : curves) {
    std::vector<DirectedIndex> indices;
    for (auto he : trace_points(arr, c.begin(), c.end())) {
      indices.push_back(he.data());
    }
    auto last = std::unique(indices.begin(), indices.end());
    indices.erase(last, indices.end());
    for (auto index : indices)
      std::cout << index.index << " " << index.fwd << " ";
    std::cout << std::endl;
  }

  return 0;
}
