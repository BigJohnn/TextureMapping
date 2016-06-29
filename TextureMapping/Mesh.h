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

#include <vector>

#include "Shader.h"
#include "GENERAL.h"
//#include "WingedEdge.h"
#include "DCELMesh.h"
#include <string.h>
#include "Initialize.h"
//#include "lscm_with_c_api.h"

#define near_plane 0.1f
#define far_plane 100.0f

using namespace std;

//extern aiVector2D* TextureCoords[N];
// I/O

struct Vertex{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords[N];
};

struct Texture {
	unsigned int id_gl;//opengl为每个纹理贴图分配的代号
	unsigned int id;//实际输入图像的编号
	std::string type;
	aiString path;
};

class Facet : public std::vector<int> {
public:
};

class VertexData
{
public:
	point3 position;
	
	/**
	If you need to save the stream to a file, rewrite the << operator for the
	ostream class. The DCELStream class will use it to write to a file. More latter.
	*/
	friend std::ostream& operator <<(std::ostream &os, const VertexData& vertexData)
	{
		double x, y, z;
		x = vertexData.position.get<0>();
		y = vertexData.position.get<1>();
		z = vertexData.position.get<2>();
		os << x << " " <<  y << " " << z;
		return os;
	}

	/**
	If you save the mesh to a file, rewrite the operator >> to be able to load
	the mesh latter using the DCELStream class.
	*/
	friend std::istream& operator >>(std::istream &is, VertexData& vertexData)
	{
		double x, y, z;
		x = vertexData.position.get<0>();
		y = vertexData.position.get<1>();
		z = vertexData.position.get<2>();
		is >> x >> y >> z;
		return is;
	}
};


class HalfEdgeData
{
public:
	HalfEdgeData() {
		modified = false;
		inited = false;
		memset(Esmooth, 0, N9*N9*sizeof(double));
	}

	void modify() {
		modified = true;
	}

	void antimodify(){
		modified = false;
	}

	bool isModified() const {
		return modified;
	}

	//初始化的是每个面元所对应的平移向量，分为9个标准向量、9个最终平移向量和N个初始向量
	void init()
	{
		inited = true;
	}

	bool isInited() const {
		return inited;
	}

	friend std::ostream& operator <<(std::ostream &os, const HalfEdgeData& halfEdgeData)
	{
		std::string modified = halfEdgeData.isModified() ? "true" : "false";
		os << modified;
		return os;
	}

	friend std::istream& operator >>(std::istream &is, HalfEdgeData& halfEdgeData)
	{
		std::string modified;
		is >> modified;

		if (modified.compare("true") == 0)
		{
			halfEdgeData.modify();
		}

		return is;
	}

private:
	bool modified;
	bool inited;

public:
	double Esmooth[N9][N9];
	//double Esmooth_l[N][N];
};

class FaceData
{
public:
	FaceData()
	{
		memset(Edata, 0, sizeof(double) * 9 * N);
	}
	

	friend std::ostream& operator <<(std::ostream &os, const FaceData& faceData)
	{
		//os << faceData.indices[0] << " " << faceData.indices[1] << " " << faceData.indices[2];
		os << faceData.colorName;
		return os;
	}

	friend std::istream& operator >>(std::istream &is, FaceData& faceData)
	{
		is >> faceData.indices[0] >> faceData.indices[1] >> faceData.indices[2];
		return is;
	}
	
public:
	double Edata[N];
	//double Edata[N][9];
	unsigned int indices[3];
	std::string colorName;
	glm::vec2 default_vec[N];
	glm::vec2 translate_vec[9];
	glm::vec2 canonical_vec[9];
};

typedef DCELMesh<VertexData, HalfEdgeData, FaceData> dcelMesh;
typedef FaceT<VertexData, HalfEdgeData, FaceData> face;
typedef HalfEdgeT<VertexData, HalfEdgeData, FaceData> halfedge;

class Mesh : public dcelMesh{
public:
	/*  Mesh Data  */
	vector<Vertex> vertices;
	vector<GLuint> indices;
	vector<Texture> textures;
	vector<Facet>  facet;
	bool in_facet;
	cv::Mat img_norm[N];
	cv::Mat cam_pos[N];
	cv::Mat modelMatrix;
	cv::Mat viewMatrices[N];
	vector<GLuint> indices4draw[N];

	/*  Functions  */
	// Constructor
	Mesh();
	Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
	Mesh(const Mesh &m);
	void construct(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);

	// Render the mesh
	void Draw(Shader shader, bool drawFBO = false);

private:
	/*  Render data  */
	GLuint VAO[N], VBO[N], EBO[N];

	/*  Functions    */
	// Initializes all the buffer objects/arrays
	void setupMesh();

public:
	//Save meshes As WaveFront .Obj ...
	void save(const std::string& file_name);
};

double ai_distance2(aiVector3D &a, aiVector3D &b)
{
	return (a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z);
}