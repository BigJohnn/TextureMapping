#include "stdafx.h"

#include "Mesh.h"
extern InputData in;
std::ostream& operator<<(std::ostream& out, const glm::vec2& v) {
	return out << v.x << " " << v.y;
}

std::ostream& operator<<(std::ostream& out, const glm::vec3& v) {
	return out << v.x << " " << v.y << " " << v.z;
}

Mesh::Mesh()
{
	this->SetUnhandledTrianglesCount(0);
}


//Copy Constructor
Mesh::Mesh(const Mesh &m)
{
	for (unsigned int a = 0; a < N; a++)
	{
		//if (m.mTextureCoords[a] != NULL)
		{
			mTextureCoords[a] = new aiVector3D[m.getVertices().size()];//此处应该于某处释放内存？
			memcpy(mTextureCoords[a], m.mTextureCoords[a], sizeof(aiVector3D)*m.getVertices().size());
		}
	}
	
	vertices = m.vertices;
	indices = m.indices;
	textures = m.textures;
	this->modelMatrix = in.modelMatrix;

	for (int i = 0; i < N; i++)
	{
		VAO[i] = m.VAO[i];
		VBO[i] = m.VBO[i];
		EBO[i] = m.EBO[i];
		indices4draw[i] = m.indices4draw[i];
		img_norm[i] = m.img_norm[i];
		cam_pos[i] = m.cam_pos[i];
		this->viewMatrices[i] = in.transfMat[i];
	}	
}

void Mesh::construct(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures)
{
	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;
	this->SetUnhandledTrianglesCount(0);
	this->modelMatrix = in.modelMatrix;
	
	for (int i = 0; i < N; i++)
	{
		this->viewMatrices[i] = in.transfMat[i];
		//cv::Mat RT = in.modelMatrix*in.transfMat[i].inv();
		cv::Mat RT = in.transfMat[i].inv()*in.modelMatrix;
		cv::Mat r(RT, cv::Rect(0, 0, 3, 3));
		
		//cv::Mat T(RT, cv::Rect(3, 0, 1, 3));
		cv::Mat T(RT, cv::Rect(3, 0, 1, 4));
		this->cam_pos[i] = T;

		cv::Mat r_inv = r.inv(); //cout << r << r_inv << endl;

		//像平面法向量
		cv::Mat img_norm(r_inv, cv::Rect(2,0, 1, 3)); //可以  因z轴坐标（0,0,1）

		//cv::Mat img_norm(r, cv::Rect(0, 2, 3, 1));  //貌似也可以。

		this->img_norm[i] = img_norm;
	}

	this->in_facet = 0;
	// Now that we have all the required data, set the vertex buffers and its attribute pointers.
	this->setupMesh();
}

void Mesh::Draw(Shader shader, bool drawFBO)
{
	// Bind appropriate textures
	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint reflectionNr = 1;

	//设备坐标系->纹理坐标系
	glm::mat4 biasMatrix(
		0.5, 0.0, 0.0, 0.0,
		0.0, 0.5, 0.0, 0.0,
		0.0, 0.0, 0.5, 0.0,
		0.5, 0.5, 0.5, 1.0
		);

	for (GLuint i = 0; i < this->textures.size(); i++)
	//int i = 1;
	{
		//float w = this->cam_pos[this->textures[i].id].at<float>(3, 0);
		glm::vec3 imageNorm(this->img_norm[this->textures[i].id].at<float>(0, 0), this->img_norm[this->textures[i].id].at<float>(1, 0), 
			this->img_norm[this->textures[i].id].at<float>(2, 0));
		glm::vec3 camPos(this->cam_pos[this->textures[i].id].at<float>(0, 0), this->cam_pos[this->textures[i].id].at<float>(1, 0), 
			this->cam_pos[this->textures[i].id].at<float>(2, 0));
		
		//cv::Mat VM = this->viewMatrices[i] * this->modelMatrix;// .inv() * this->modelMatrix.inv();
		cv::Mat VM = this->viewMatrices[i].inv() * this->modelMatrix.inv();

		cv::Mat vVM(VM, cv::Rect(3, 0, 1, 4));

		glm::vec3 cam(vVM.at<float>(0, 0), vVM.at<float>(1, 0), vVM.at<float>(2, 0));

		// Compute the MVP matrix from the light's point of view
		glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, near_plane, far_plane);

		glm::vec3 origin(0, 0, 0);
		glm::mat4 depthViewMatrix = glm::lookAt(camPos, origin, glm::vec3(0, 1, 0));//glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		
		glm::mat4 depthModelMatrix;
		
		depthModelMatrix = glm::scale(depthModelMatrix, glm::vec3(0.01f, 0.01f, 0.01f));	// It's a bit too big for our scene, so scale it down
		glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

		
		if (drawFBO == false)
		{
			
			glActiveTexture(GL_TEXTURE0 + i); // Active proper texture unit before binding
			// Retrieve texture number (the N in diffuse_textureN)
			stringstream ss;
			string number;
			string name = this->textures[i].type;
			if (name == "texture_diffuse")
				ss << diffuseNr++; // Transfer GLuint to stream
			
			number = ss.str();
			// Now set the sampler to the correct texture unit
			glUniform1i(glGetUniformLocation(shader.Program, ( name).c_str()), i);

			glUniform3fv(glGetUniformLocation(shader.Program, "imgNorm" ), 1, glm::value_ptr(imageNorm));

			glUniform3fv(glGetUniformLocation(shader.Program, "camPos"), 1, glm::value_ptr(camPos));

			// And finally bind the texture
			glBindTexture(GL_TEXTURE_2D, i+1);
			
			glm::mat4 depthBiasMVP = biasMatrix*depthMVP;
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "DepthBiasMVP"), 1, GL_FALSE, glm::value_ptr(depthBiasMVP));

			//zhongyao
			int d = this->textures.size() + i;
			glActiveTexture(GL_TEXTURE0 + d);
			glBindTexture(GL_TEXTURE_2D, in.depthTexture[i]);
			glUniform1i(glGetUniformLocation(shader.Program, "shadowMap"), d);

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
			glBindVertexArray(this->VAO[textures[i].id]);
			glDrawElements(GL_TRIANGLES, this->indices4draw[textures[i].id].size(), GL_UNSIGNED_INT, 0);
			glCullFace(GL_FRONT); // Cull back-facing triangles -> draw only front-facing triangles
			glBindVertexArray(0);
		}
		else
		{
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, in.depthMapFBO[i]);
			
			// Clear the screen
			glClear(GL_DEPTH_BUFFER_BIT);

			glm::mat4 shadowProj = glm::perspective(90.0f, 1.0f, 1.0f, 25.0f);

			glm::mat4 depthMVPD = shadowProj *
				glm::lookAt(camPos, camPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));
			glUniformMatrix4fv(glGetUniformLocation(shader.Program, "depthMVP"), 1, GL_FALSE, glm::value_ptr(depthMVPD));
			
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK); // Cull back-facing triangles -> draw only front-facing triangles
			glBindVertexArray(this->VAO[textures[i].id]);
			glDrawElements(GL_TRIANGLES, this->indices4draw[textures[i].id].size(), GL_UNSIGNED_INT, 0);
			glCullFace(GL_FRONT);
			glBindVertexArray(0);
			
		}
		glActiveTexture(GL_TEXTURE0);	
	}
}

void Mesh::setupMesh()
{
	glGenVertexArrays(N, this->VAO);
	glGenBuffers(N, this->VBO);
	glGenBuffers(N, this->EBO);
	// Create buffers/arrays
	for (int i = 0; i < N; i++)
	{
		glBindVertexArray(this->VAO[i]);
		// Load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO[i]);

		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO[i]);

		if (this->indices4draw[i].size() == 0)
		{
			this->indices4draw[i].push_back(0);//error!
		}
			
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices4draw[i].size() * sizeof(GLuint), &this->indices4draw[i][0], GL_STATIC_DRAW);
		//glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

		// Set the vertex attribute pointers
		// Vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
		// Vertex Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));
		// Vertex Texture Coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords[i]));

		glBindVertexArray(0);
	}
}

