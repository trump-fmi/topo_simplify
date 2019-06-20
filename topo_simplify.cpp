#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyline_simplification_2/simplify.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <vector>

namespace PS = CGAL::Polyline_simplification_2;

typedef CGAL::Exact_predicates_exact_constructions_kernel K;
typedef CGAL::Exact_intersections_tag Itag;
typedef PS::Vertex_base_2<K> Vb;
typedef CGAL::Constrained_triangulation_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> TDS;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag> CDT;
typedef CGAL::Constrained_triangulation_plus_2<CDT> CDTplus;
typedef CDTplus::Point Point;
typedef PS::Stop_above_cost_threshold Stop;
typedef PS::Squared_distance_cost Cost;

int main() {
  CDTplus cdt;
  double eps;

  std::vector<CDTplus::Constraint_id> handles;
  std::string line;

  // eps
  {
    std::getline(std::cin, line);
    std::stringstream linestream(line);
    linestream >> eps;
  }

  // points
  {
    std::getline(std::cin, line);
    std::stringstream linestream(line);
    auto points_begin = std::istream_iterator<Point>(linestream);
    auto points_end = std::istream_iterator<Point>();
    cdt.insert(points_begin, points_end);
  }

  // constraints
  while (std::getline(std::cin, line)) {
    std::stringstream linestream(line);
    auto points_begin = std::istream_iterator<Point>(linestream);
    auto points_end = std::istream_iterator<Point>();
    auto ch = cdt.insert_constraint(points_begin, points_end);
    handles.push_back(ch);
  }

  PS::simplify(cdt, Cost(), Stop(eps * eps));

  // output
  for (auto cid : handles) {
    auto vertices_begin = cdt.vertices_in_constraint_begin(cid);
    auto vertices_end = cdt.vertices_in_constraint_end(cid);
    auto output = std::ostream_iterator<Point>(std::cout, " ");
    std::transform(vertices_begin, vertices_end, output,
                   [](auto v) { return v->point(); });
    std::cout << std::endl;
  }

  return 0;
}
