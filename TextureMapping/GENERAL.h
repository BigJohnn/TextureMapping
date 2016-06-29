#include "stdafx.h"

namespace bg = boost::geometry;
//typedef bg::model::point<glm::vec2, 1, bg::cs::cartesian> point1;
// Calculate the area of a cartesian polygon
typedef bg::model::point<double, 2, bg::cs::cartesian> point2;
typedef bg::model::point<double, 3, bg::cs::cartesian> point3;
typedef bg::model::polygon<point2, false> triangle2;
typedef bg::model::polygon<point3, false> triangle3;
typedef bg::model::segment<point2> segment2;
typedef bg::model::segment<point3> segment3;
typedef bg::model::box<point2> rectange;

