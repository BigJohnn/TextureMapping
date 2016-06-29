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

#include "Mesh.h"
#include "DCELStream.h"

using boost::geometry::append;

GLint TextureFromFile(cv::Mat image);

GLint TextureFromFile(const char* path, string directory);

void saveMesh(dcelMesh* mesh, const char* filename);

int loadMesh(dcelMesh* mesh, const char* filename);

class Model
{
public:
	Model(std::string path)
	{
		this->loadModel(path);
	}

	void Draw(Shader shader, bool drawFBO = false)
	{
		for (GLuint i = 0; i < this->meshes.size(); i++)
		{
			this->meshes[i].Draw(shader, drawFBO);
		}	
	}

	void loadModel(std::string path);


private:
	std::string directory;
	vector<Texture> textures_loaded;

	// Processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this 
	// process on its children nodes (if any).
	void processNode(aiNode* node, const aiScene* scene);

	Mesh processMesh(aiMesh* mesh, const aiScene* scene);

public:
	std::vector<Mesh> meshes;
};

template <class MeshT>
class DCELImporter
{
public:
	void import(std::list<point3>& vertices, unsigned int verticeCount, std::list<unsigned int>& faces, unsigned int faceCount, MeshT& mesh);

};

