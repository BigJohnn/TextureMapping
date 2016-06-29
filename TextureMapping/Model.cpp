#include "stdafx.h"

#pragma once

#include "Model.h"
#include "GraphProcess.h"
#include <exception>

extern InputData in;

void Model::loadModel(std::string path)
{
	// Read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs 
		| aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality);
	if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	// Retrieve the directory path of the filepath
	this->directory = path.substr(0, path.find_last_of('\\'));

	this->processNode(scene->mRootNode, scene);
}


void Model::processNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// The node object only contains indices to index the actual objects in the scene. 
		// The scene contains all the data, node is just to keep stuff organized.
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		this->meshes.push_back(this->processMesh(mesh, scene));
	}
	// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		// Child nodes are actually stored in the node, not in the scene (which makes sense since nodes only contain
		// links and indices, nothing more, so why store that in the scene)
		this->processNode(node->mChildren[i], scene);
	}

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture> textures;

	std::list<point3> verts;
	std::list<unsigned int> faces;
	
	Mesh *dc_mesh = new Mesh();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		verts.push_back(point3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
	}

	// Now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (GLuint i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// Retrieve all indices of the face and store them in the indices vector
		for (GLuint j = 0; j < face.mNumIndices; j++)
		{
			faces.push_back((unsigned int)face.mIndices[j]);
			indices.push_back(face.mIndices[j]);
		}
	}
	
	if (-1 == loadMesh(dc_mesh, "table_cup.dcel"))
	{
		//which takes 136 seconds.
		DCELImporter<dcelMesh> importer;
		importer.import(verts, mesh->mNumVertices, faces, mesh->mNumFaces, *dc_mesh);
		saveMesh(dc_mesh, "table_cup.dcel");
	}
	
	/*for(int i=0;i<N;i++)
	{
		cv::imshow("请选择目标区域。", in.pyramids[i][1].src);
		cv::waitKey(0);
	}*/


	//Ivonov etc. 2007
	textures = FaceTraverse0(dc_mesh);

	//Gal etc. 2010
	//textures = FaceTraverse(dc_mesh);

	for (unsigned int i = 0; i < dc_mesh->getVertices().size(); i++)
	{
		Vertex vertex;
		glm::vec3 vector;
		
		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		vertex.Position = vector;

		vector.x = mesh->mNormals[i].x;
		vector.y = mesh->mNormals[i].y;
		vector.z = mesh->mNormals[i].z;
		vertex.Normal = vector;

		// Texture Coordinates
		for (int j = 0; j < N; j++)//..............................N <->1
		{
			glm::vec2 vec;
			vec.x = dc_mesh->mTextureCoords[j][i].x;
			vec.y = dc_mesh->mTextureCoords[j][i].y;

			vertex.TexCoords[j] = vec;
		}
		
		vertices.push_back(vertex);
	}

	dc_mesh->construct(vertices, indices, textures);
	// Return a mesh object created from the extracted mesh data
	return *dc_mesh;
}

GLuint TextureFromFile(cv::Mat image)
{
	//Generate texture ID and load texture data 
	GLuint textureID;
	glGenTextures(1, &textureID);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//use fast 4-byte alignment (default anyway) if possible
	glPixelStorei(GL_UNPACK_ALIGNMENT, (image.step & 3) ? 1 : 4);
	//set length of one complete row in data (doesn't need to equal image.cols)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step / image.elemSize());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

template <class MeshT>
void DCELImporter<MeshT>::import(std::list<point3>& vertices, unsigned int verticeCount, std::list<unsigned int>& faces, unsigned int faceCount, MeshT& mesh)
{
	// put it into the mesh
	std::cerr << "- loading the DCEL mesh: " << std::endl;
	std::cerr << "  + " << verticeCount << " vertices" << std::endl;
	mesh.clear();
	mesh.getVertices().reserve(verticeCount);
	mesh.getFaces().reserve(faceCount);
	//mesh.getEdges().reserve( 2*(verticeCount+faceCount) ); // Euler: V ?A + F = 2 --> A = V + F - 2
	mesh.getHalfEdges().reserve(2 * (3 * faceCount)); // just a good expensive number of edges =)
	std::list<point3>::iterator vit = vertices.begin();
	while (vit != vertices.end())
	{
		unsigned int id = mesh.createVertex();
		mesh.getVertex(id)->getData().position = *vit;
		vit++;
	}

	std::cerr << "  + " << faceCount << " faces" << std::endl;
	std::list<unsigned int>::iterator fit = faces.begin();
	unsigned int count = 0;
	while (fit != faces.end())
	{
		count++;
		unsigned int a = *fit; ++fit;
		unsigned int b = *fit; ++fit;
		unsigned int c = *fit; ++fit;
		unsigned int fid = mesh.createTriangularFace(a, b, c);
		if (fid != -1)
		{
			//std::cout << a << ' ' << b << ' ' << c << std::endl;
			mesh.checkFace(fid);
			mesh.checkAllFaces();
		}
	}

	std::cerr << "  + " << mesh.getNumUnhandledTriangles() << " bad triangles" << std::endl;
	mesh.manageUnhandledTriangles();
	mesh.checkAllFaces();

	std::cerr << "Done!" << std::endl;
}

//Save as dcel(half edge structure).
void saveMesh(dcelMesh* mesh, const char* filename)
{
	if (mesh != NULL)
	{
		std::ofstream outputFile;
		outputFile.open(filename);
		if (outputFile)
		{
			/**
			This is where I will write the mesh to a file.

			Note that the operator "<<" need to be rewrited on the VertexData,
			HalfEdgeData and FaceData classes.
			*/
			DCELStream<dcelMesh>::write(*mesh, outputFile);

			outputFile.close();
		}
	}
}

//Refer to the function above.
int loadMesh(dcelMesh* mesh, const char* filename)
{
	if (mesh != NULL)
	{
		std::ifstream inputFile;
		inputFile.open(filename);
		if (inputFile)
		{
			/*
			This is where I load the mesh from a DCEL file, saved with the
			DCELStream<MyMesh>::write() method.

			As for the DCELStream<MyMesh>::write(), here you need to rewrite the
			">>" operator on the VertexData, HalfEdgeData and FaceData classes.
			*/
			DCELStream<dcelMesh>::read(*mesh, inputFile);

			inputFile.close();
			return 0;
		}
		else return -1;
	}
	return -1;
}