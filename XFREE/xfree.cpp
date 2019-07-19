#include <iostream>
#include <vector>

#include <CGAL/Arr_batched_point_location.h>
#include <CGAL/Arr_extended_dcel.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Arrangement_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Point_2 = K::Point_2;
typedef CGAL::Arr_segment_traits_2<> Geom_traits_2;
typedef CGAL::Arr_extended_dcel<Geom_traits_2, bool, bool, int>
    Dcel; // Vertex, HE, Face
typedef CGAL::Arrangement_2<Geom_traits_2, Dcel> Arrangement_2;
using Segment_2 = Geom_traits_2::X_monotone_curve_2;

using Polyline = std::vector<Point_2>;

struct Arrangement {
  std::vector<Point_2> points;
  std::vector<Polyline> curves;
};

Arrangement read_instance(std::istream &in) {
  Arrangement arr;

  std::string line;
  std::getline(in, line);
  std::stringstream linestream(line);
  std::copy(std::istream_iterator<Point_2>(linestream), {},
            std::back_inserter(arr.points));

  size_t N;
  in >> N;
  in.ignore();

  for (size_t i = 0; i < N; ++i) {
    std::getline(in, line);
    linestream = std::stringstream(line);

    Polyline curve;
    std::copy(std::istream_iterator<Point_2>(linestream), {},
              std::back_inserter(curve));
    arr.curves.push_back(curve);
  }

  return arr;
}

Arrangement make_xfree(Arrangement &arr) {
  Arrangement_2 cgal_arr;
  std::vector<Arrangement_2::Vertex_handle> stop_vertices;

  for (auto p : arr.points) {
    CGAL::insert_point(cgal_arr, p);
  }
  for (auto v : cgal_arr.vertex_handles()) {
    stop_vertices.push_back(v);
  }

  std::vector<Segment_2> segments;
  std::vector<Point_2> endpoints;
  for (auto &c : arr.curves) {
    for (size_t i = 1; i < c.size(); ++i) {
      segments.emplace_back(c[i - 1], c[i]);
    }
    endpoints.push_back(c.front());
    endpoints.push_back(c.back());
  }
  CGAL::insert(cgal_arr, segments.begin(), segments.end());
  typedef CGAL::Arr_point_location_result<Arrangement_2> Res;
  std::vector<std::pair<Point_2, Res::Type>> locations;
  CGAL::locate(cgal_arr, endpoints.begin(), endpoints.end(),
               std::back_inserter(locations));
  for (auto &res : locations) {
    auto *cvh = boost::get<Arrangement_2::Vertex_const_handle>(&res.second);
    auto vh = cgal_arr.non_const_handle(*cvh);
    stop_vertices.push_back(vh);
  }

  for (auto v : cgal_arr.vertex_handles()) {
    if (v->degree() != 2)
      stop_vertices.push_back(v);
  }

  for (auto v : stop_vertices) {
    v->data() = true; 
  }

  for (auto heh : cgal_arr.halfedge_handles()) {
    heh->data() = false;
  }

  std::vector<Polyline> curves;
  for (auto heh : cgal_arr.halfedge_handles()) {
    if (heh->data() || heh->twin()->data())
      continue;
    while (!heh->source()->data())
      heh = heh->prev();
    Polyline p;
    p.push_back(heh->source()->point());
    do {
      p.push_back(heh->target()->point());
      heh->data() = true;
      heh->twin()->data() = true;
      heh = heh->next();
    } while (!heh->source()->data());
    curves.push_back(p);
  }

  Arrangement xarr;
  xarr.points = arr.points;
  xarr.curves = curves;
  return xarr;
}

int main(int argc, char *argv[]) {
  Arrangement arr = read_instance(std::cin);
  Arrangement xarr = make_xfree(arr);

  std::copy(xarr.points.begin(), xarr.points.end(),
            std::ostream_iterator<Point_2>(std::cout, " "));
  std::cout << std::endl;

  std::cout << xarr.curves.size() << std::endl;
  for (auto &c : xarr.curves) {
    std::copy(c.begin(), c.end(),
              std::ostream_iterator<Point_2>(std::cout, " "));
    std::cout << std::endl;
  }

  return 0;
}
