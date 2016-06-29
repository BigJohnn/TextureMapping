// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
//boost
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/property_map/property_map.hpp>
//#include <boost/graph/graph_utility.hpp>
#include <boost/pending/indirect_cmp.hpp>
#include <boost/range/irange.hpp>

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/segment.hpp>
#include <boost/geometry/geometries/box.hpp>
#include <boost/geometry/geometries/linestring.hpp>
#include <boost/foreach.hpp>
//GLEW
#include <gl/glew.h> // Contains all the necessery OpenGL includes

// GLFW
#include <gl/glfw3.h>

//glm
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <afxwin.h> //to use CString

#include <vector>

//opencv
#include <opencv\cv.hpp>
#include <opencv2\opencv.hpp>

//assimp
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <gl/SOIL.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//The Number of ViewPoints
#define N 4

#define N9 N*9

#define FLOOR_NUM 6

#define NUM_FIRST 2
#define NUM_SECOND 4
