#include "stdafx.h"
#include "Geometry.h"

extern segment3 edge3;
extern InputData in;

bool operator==(const point3 &a, const point3 &b) {
	return (a.get<0>() == b.get<0>())
		&& (a.get<1>() == b.get<1>())
		&& (a.get<2>() == b.get<2>());
}
bool operator!=(const point3 &a, const point3 &b) {
	return (a.get<0>() != b.get<0>())
		|| (a.get<1>() != b.get<1>())
		|| (a.get<2>() != b.get<2>());
}

void translationTransform(cv::Mat &src, cv::Mat& dst, glm::vec2 pt)
{
	float data[6] = { 1, 0, pt.x, 0, 1, pt.y };
	cv::Mat M(2, 3, CV_32FC1, data);
	cv::warpAffine(src, dst, M, cv::Size(src.cols, src.rows));
}

triangle3 CreateTriangle3(face *f)
{
	triangle3 triangle;
	append(triangle, f->getBoundary()->getPrev()->getOrigin()->getData().position);
	append(triangle, f->getBoundary()->getOrigin()->getData().position);
	append(triangle, f->getBoundary()->getNext()->getOrigin()->getData().position);
	append(triangle, f->getBoundary()->getPrev()->getOrigin()->getData().position);
	return triangle;
}

triangle3 createTriangle3(aiFace &face, aiMesh*mesh)
{
	triangle3 triangle;
	append(triangle, point3(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z));
	append(triangle, point3(mesh->mVertices[face.mIndices[1]].x, mesh->mVertices[face.mIndices[1]].y, mesh->mVertices[face.mIndices[1]].z));
	append(triangle, point3(mesh->mVertices[face.mIndices[2]].x, mesh->mVertices[face.mIndices[2]].y, mesh->mVertices[face.mIndices[2]].z));
	append(triangle, point3(mesh->mVertices[face.mIndices[0]].x, mesh->mVertices[face.mIndices[0]].y, mesh->mVertices[face.mIndices[0]].z));
	return triangle;
}

cv::Point3f point3_Point3f(point3 &pt3)
{
	return cv::Point3f(pt3.get<0>(), pt3.get<1>(), pt3.get<2>());
}
cv::Point3f vec3topt3f(glm::vec3 &vec)
{
	cv::Point3f pt;
	pt.x = vec.x;
	pt.y = vec.y;
	pt.z = vec.z;
	return pt;
}

point2 Point2f_point2(cv::Point2f &pt2f)
{
	return point2(pt2f.x, pt2f.y);
}
glm::vec2 pt2ftovec2(cv::Point2f &pt)
{
	glm::vec2 vec;
	vec.x = pt.x;
	vec.y = pt.y;
	return vec;
}

triangle2 model3D_ImageT2D(triangle3 &t3, cv::Mat R, cv::Mat T)
{
	triangle2 t2;

	std::vector<cv::Point3f> vecs;
	std::vector<cv::Point2f> vec2s;
	for (int i = 0; i < 4; i++)
	{
		vecs.push_back(point3_Point3f(t3.outer().at(i)));
	}
	cv::projectPoints(vecs, R, T, in.cameraMatrix, in.distCoeffs, vec2s);

	for (int i = 0; i < 4; i++)
	{
		append(t2, point2(vec2s[i].x, vec2s[i].y));
	}

	return t2;
}

segment2 seg3_seg2(segment3 &seg3, cv::Mat R, cv::Mat T)
{
	segment2 seg2;
	std::vector<cv::Point3f> vecs;
	std::vector<cv::Point2f> vec2s;
	vecs.push_back(point3_Point3f(seg3.first));
	vecs.push_back(point3_Point3f(seg3.second));
	cv::projectPoints(vecs, R, T, in.cameraMatrix, in.distCoeffs, vec2s);
	seg2.first = Point2f_point2(vec2s[0]);
	seg2.second = Point2f_point2(vec2s[1]);
	return seg2;
}