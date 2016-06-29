#include "stdafx.h"

#include "Initialize.h"
#include "Model.h"

#define ABS_FLOAT_0 1

#include "GENERAL.h"

void translationTransform(cv::Mat &src, cv::Mat& dst, glm::vec2 pt);

triangle3 CreateTriangle3(face *f);

cv::Point3f point3_Point3f(point3 &pt3);

point2 Point2f_point2(cv::Point2f &pt2f);

triangle2 model3D_ImageT2D(triangle3 &t3, cv::Mat R, cv::Mat T);

segment2 seg3_seg2(segment3 &seg3, cv::Mat R, cv::Mat T);