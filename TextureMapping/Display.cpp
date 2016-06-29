#include "stdafx.h"
#include "Display.h"

extern GLuint screenWidth = 800, screenHeight = 600;
extern GLFWwindow* window;
extern Camera camera;
extern GLfloat deltaTime;
extern GLfloat lastFrame;
extern InputData in;

void Display(Model mdl)
{
	Shader shader("target.vert", "target.frag");
	Shader shadowshader("ps.vert", "ps.frag");
	Shader screenshader("screen.vert", "screen.frag");

	// Draw in wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Configure depth map FBO
	glGenFramebuffers(N, in.depthMapFBO);
	
	// Depth texture. Slower than a depth buffer, but you can sample it later in your shader
	//GLuint depthTexture;
	glGenTextures(N, in.depthTexture);
	for (int i = 0; i < N; i++)
	{
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, in.depthMapFBO[i]);
		glBindTexture(GL_TEXTURE_2D, in.depthTexture[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, in.depthTexture[i], 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		// Always check that our framebuffer is ok
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			cout << "The framebuffer is not OK" << endl;

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
	//glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, in.depthTexture, 0);//此方法适用性更广，如cubemap
	

	glClearColor(0.996f, 0.875f, 0.882f, 1.0f);//樱
	//glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
	while (!glfwWindowShouldClose(window))
	{
		// Set frame time
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Check and call events
		glfwPollEvents();
		Do_Movement();

		
		//1. Render to our framebuffer object
		glViewport(0, 0, 1024, 1024); // Render on the whole framebuffer, complete from the lower left corner to the upper right
		/*glClear(GL_DEPTH_BUFFER_BIT);*/
		shadowshader.Use();
		mdl.Draw(shadowshader, true);

		//2. Render to screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);//用完FBO要解绑，养成良好的习惯
		glViewport(0, 0, screenWidth, screenHeight);
		//glEnable(GL_DEPTH_TEST);
		// Clear the colorbuffer 
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.Use();
		
		// Transformation matrices 
		glm::mat4 projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, near_plane, far_plane);
		glm::mat4 view = camera.GetViewMatrix();
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));

		// Draw the loaded model
		glm::mat4 model;
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));	// It's a bit too big for our scene, so scale it down
		glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));

		mdl.Draw(shader);

		//debug.

		//
		//glViewport(0, 0, 512, 512);
		//glEnable(GL_CULL_FACE);
		//
		//glCullFace(GL_BACK);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//screenshader.Use();
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, in.depthTexture);
		//glUniform1i(glGetUniformLocation(screenshader.Program, "screenTexture"), 0);
		//GLfloat quadVertices[] = {
		//	// Positions        // Texture Coords
		//	-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		//	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		//	1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		//	1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		//};
		//GLuint quadVAO;
		//GLuint quadVBO;
		//// Setup plane VAO
		//glGenVertexArrays(1, &quadVAO);
		//glGenBuffers(1, &quadVBO);
		//glBindVertexArray(quadVAO);
		//glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		//glEnableVertexAttribArray(0);
		//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		//
		//glBindVertexArray(quadVAO);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//glBindVertexArray(0);
		//shader.Disable();

		glfwSwapBuffers(window);
	}
	glfwTerminate();
}
