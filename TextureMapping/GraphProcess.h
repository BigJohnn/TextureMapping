//Copyright(c) 2016, Hou Peihong
//All rights reserved.
//
//Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met :
//
//1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
//2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and / or other materials provided with the distribution.
//
//3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
//THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
//BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
//GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
//LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#include "stdafx.h"
#pragma once
#include <queue>
#include "TRW_S.h"

#include "Geometry.h"
#include "Mesh.h"
#include "Display.h"
#include <boost/progress.hpp>

#define LAST_FLOOR_TO_PROCESS 5
//
std::vector<int> global_texture_loaded;
std::vector<Texture> global_textures;
pair<int, int> translation[9];

enum Object4Process
{
	normal,
	single_face,
	holes
};

struct node{
	float data;
	int order;
};
int cmp(const void *a, const void *b)
{
	return (*(struct node *)a).data - (*(struct node *)b).data;
}

std::vector<Texture> Compute(Mesh* mesh);
std::vector<Texture> FaceTraverse0(Mesh* mesh);
std::vector<Texture> FaceTraverse(Mesh* mesh);

//DDA related.
struct EdgeInfoSmooth
{
	segment2 edge;
	cv::Mat texture0, texture1;
	double Esmooth;
	int multiple;

	//bias
	int left;//0~8
	int right;//0~8
};

VOID CALLBACK EsmoothOnEdge0(int x, int y, EdgeInfoSmooth &edgeinfo)
{
	cv::Vec3i dist = (cv::Vec3d)edgeinfo.texture0.at<cv::Vec3b>(y, x) - (cv::Vec3d)edgeinfo.texture1.at<cv::Vec3b>(y, x); //(*data0) - (*data1);// 
	edgeinfo.Esmooth += dist.dot(dist);
}
VOID CALLBACK EsmoothOnEdge(int x, int y, EdgeInfoSmooth &edgeinfo)
{
	cv::Vec3i dist = (cv::Vec3d)edgeinfo.texture0.at<cv::Vec3b>(y+translation[edgeinfo.left].second, x+translation[edgeinfo.left].first)
		- (cv::Vec3d)edgeinfo.texture1.at<cv::Vec3b>(y + translation[edgeinfo.right].second, x + translation[edgeinfo.right].first); //(*data0) - (*data1);// 
	edgeinfo.Esmooth += dist.dot(dist);
}

inline int min3num(float &a, float &b, float &c)
{
	return (int)std::min(std::min(a, b), c);
}

inline int max3num(float &a, float &b, float &c)
{
	return (int)std::max(std::max(a, b), c);
}

//overloadED..............................................................
inline double min3num(const double &a, const double &b, const double &c)
{
	return std::min(std::min(a, b), c);
}

inline double max3num(const double &a, const double &b, const double &c)
{
	return std::max(std::max(a, b), c);
}

//Single Triangle
inline double findMinX(triangle2 T)
{
	return min3num(T.outer().at(0).get<0>(), T.outer().at(1).get<0>(), T.outer().at(2).get<0>());
}

inline double findMinY(triangle2 T)
{
	return min3num(T.outer().at(0).get<1>(), T.outer().at(1).get<1>(), T.outer().at(2).get<1>());
}

inline double findMaxX(triangle2 T)
{
	return max3num(T.outer().at(0).get<0>(), T.outer().at(1).get<0>(), T.outer().at(2).get<0>());
}

inline double findMaxY(triangle2 T)
{
	return max3num(T.outer().at(0).get<1>(), T.outer().at(1).get<1>(), T.outer().at(2).get<1>());
}

//Double Triangles
inline double findMinX(triangle2 T1, triangle2 T2)
{
	return std::min(findMinX(T1), findMinX(T2));
}

inline double findMinY(triangle2 T1, triangle2 T2)
{
	return std::min(findMinY(T1), findMinY(T2));
}

inline double findMaxX(triangle2 T1, triangle2 T2)
{
	return std::max(findMaxX(T1), findMaxX(T2));
}

inline double findMaxY(triangle2 T1, triangle2 T2)
{
	return std::max(findMaxY(T1), findMaxY(T2));
}