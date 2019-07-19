#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Constrained_triangulation_plus_2.h>

#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Polyline_simplification_2/simplify.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdlib.h>
#include <vector>

namespace PS = CGAL::Polyline_simplification_2;

using K = CGAL::Exact_predicates_exact_constructions_kernel;
using Itag = CGAL::Exact_intersections_tag;
using VbI = CGAL::Triangulation_vertex_base_with_info_2<double, K>;
using Vb = PS::Vertex_base_2<K, VbI>;
using Fb = CGAL::Constrained_triangulation_face_base_2<K>;
using TDS = CGAL::Triangulation_data_structure_2<Vb, Fb>;
using CDT = CGAL::Constrained_Delaunay_triangulation_2<K, TDS, Itag>;
using CDTplus = CGAL::Constrained_triangulation_plus_2<CDT>;
using Point = CDTplus::Point;
using Stop = PS::Stop_above_cost_threshold;
using Cost = PS::Squared_distance_cost;

int main(int argc, char **argv) {
  CDTplus cdt;
  std::vector<double> eps_vals;

  // eps
  {
    std::transform(argv + 1, argv + argc, std::back_inserter(eps_vals),
                   [](char *arg) { return std::stod(arg); });
  }

  std::vector<CDTplus::Constraint_id> handles;
  std::string line;

  // points
  std::vector<Point> points;
  {
    std::getline(std::cin, line);
    std::stringstream linestream(line);
    auto points_begin = std::istream_iterator<Point>(linestream);
    auto points_end = std::istream_iterator<Point>();
    points = {points_begin, points_end};
    cdt.insert(points_begin, points_end);
  }

  // N
  size_t N;
  {
    std::getline(std::cin, line);
    std::stringstream linestream(line);
    linestream >> N;
  }

  // constraints
  for (size_t i = 0; i < N; ++i) {
    std::getline(std::cin, line);
    std::stringstream linestream(line);
    auto points_begin = std::istream_iterator<Point>(linestream);
    auto points_end = std::istream_iterator<Point>();
    auto ch = cdt.insert_constraint(points_begin, points_end);
    handles.push_back(ch);
  }

  std::copy(points.begin(), points.end(),
            std::ostream_iterator<Point>(std::cout, " "));
  std::cout << std::endl;

  std::copy(eps_vals.begin(), eps_vals.end(),
            std::ostream_iterator<double>(std::cout, " "));
  std::cout << std::endl;

  std::cout << N << std::endl;

  for (auto eps : eps_vals) {
    PS::simplify(cdt, Cost(), Stop(eps * eps), false);
    // output
    for (auto cid : handles) {
      auto vertices_begin = cdt.vertices_in_constraint_begin(cid);
      auto vertices_end = cdt.vertices_in_constraint_end(cid);
      auto output = std::ostream_iterator<Point>(std::cout, " ");
      std::transform(vertices_begin, vertices_end, output,
                     [](auto v) { return v->point(); });
      std::cout << std::endl;
    }
  }

  return 0;
}
