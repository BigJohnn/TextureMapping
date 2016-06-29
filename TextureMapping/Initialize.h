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
#pragma once

#include "UserInterface.h"

struct TextureImage
{
	CString name;
	cv::Mat src;
};

struct Label
{
	std::vector<TextureImage> sourceImage;
};

struct InputData
{
	Label label;
	TextureImage pyramids[N][FLOOR_NUM];
	cv::Mat modelMatrix;//[image plane 2 model] transform matrix
	cv::Mat cameraMatrix;
	cv::Mat distCoeffs;
	int numFloors;
	cv::Mat transfMat[N];//save N translation matrices of N images.
	GLuint planeVAO;
	GLuint depthTexture[N];
	GLuint depthMapFBO[N];
};



//Together
int init();

//GL
int InitiateGLFW();
int InitiateGLEW();

//DATA
int InitiateData();
Label CreateLabel(CString path);


void createGaussianPyramid();
void CreateGaussianPyramid(TextureImage img, std::vector<TextureImage> &Pyramid);

int LoadData(std::string fileName, cv::Mat& matData, int matRows, int matCols, int matChns);
void readCCamIntriParams(std::string dotYAML, cv::Mat &cameraMatrix, cv::Mat &distCoeffs);
