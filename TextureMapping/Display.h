#include "stdafx.h"
#pragma once

#include "Shader.h"
#include "Model.h"
#include "Initialize.h"

GLuint compute_vao;
GLuint compute_vbos[2];

enum
{
	PARTICLE_GROUP_SIZE = 128,
	PARTICLE_GROUP_COUNT = 8000,
	PARTICLE_COUNT = (PARTICLE_GROUP_SIZE * PARTICLE_GROUP_COUNT),
	MAX_ATTRACTORS = 64
};

// Compute program
GLuint  compute_prog;
GLint   dt_location;

// Posisition and velocity buffers
static union
{
	struct
	{
		GLuint position_buffer;
		GLuint velocity_buffer;
	};
	GLuint buffers[2];
};

// TBOs
static union
{
	struct
	{
		GLuint position_tbo;
		GLuint velocity_tbo;
	};
	GLuint tbos[2];
};

// Attractor UBO
GLuint  attractor_buffer;

// Program, vao and vbo to render a full screen quad
GLuint  render_prog;
GLuint  render_vao;
GLuint  render_vbo;

// Mass of the attractors
float attractor_masses[MAX_ATTRACTORS];

float aspect_ratio;

static inline float random_float()
{
	float res;
	unsigned int tmp;
	static unsigned int seed = 0xFFFF0C59;

	seed *= 16807;

	tmp = seed ^ (seed >> 4) ^ (seed << 15);

	*((unsigned int *)&res) = (tmp >> 9) | 0x3F800000;

	return (res - 1.0f);
}

static glm::vec3 random_vector(float minmag = 0.0f, float maxmag = 1.0f)
{
	glm::vec3 randomvec(random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f, random_float() * 2.0f - 1.0f);
	randomvec = glm::normalize(randomvec);
	randomvec *= (random_float() * (maxmag - minmag) + minmag);

	return randomvec;
}

//Display Func
void Display(Model model);
void ImageProcess_Compute();
void Compute_Display_particles();