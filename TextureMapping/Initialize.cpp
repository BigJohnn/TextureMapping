#include "stdafx.h"

#include "Initialize.h"

// Properties
GLuint screenWidth = 800, screenHeight = 600;
CString img_path = "images\\";
std::string translate_mat_dir = "transform\\";
GLFWwindow* window;
InputData in;

int InitiateGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	window = glfwCreateWindow(screenWidth, screenHeight, "TextureMapping", nullptr, nullptr); // Windowed
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// Options
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	return EXIT_SUCCESS;
}

int InitiateGLEW()
{
	glewExperimental = GL_TRUE;
	glewInit();
	return EXIT_SUCCESS;
}

int InitiateData()
{
	in.label = CreateLabel(img_path);

	std::string model_mat_filepath =
		"transform\\transform.txt";

	createGaussianPyramid();
	
	if (-1 == LoadData(model_mat_filepath, in.modelMatrix, 4, 4, 1))
	{
		std::cout << "ERROR: Readfile valid." << std::endl;
	}

	std::string color_IntrinParam_yml =
		"camera\\calib_color.yaml";
	readCCamIntriParams(color_IntrinParam_yml, in.cameraMatrix, in.distCoeffs);

	return EXIT_SUCCESS;
}

void readCCamIntriParams(std::string dotYAML, cv::Mat &cameraMatrix, cv::Mat &distCoeffs)
{
	cv::FileStorage fs(dotYAML, cv::FileStorage::READ);

	fs["cameraMatrix"] >> cameraMatrix;
	fs["distortionCoefficients"] >> distCoeffs;

	fs.release();
}

Label CreateLabel(CString path)
{
	Label lbl;
	CFileFind finder;
	BOOL working = finder.FindFile(path + "*.jpg");
	while (working)
	{
		working = finder.FindNextFile();
		if (finder.IsDots())
			continue;

		//ImageName[k]<-->sourceImage[k]
		CString filename = finder.GetFileName();
		TextureImage I;
		I.name = filename;

		filename = path + filename;
		USES_CONVERSION;
		char* imgpath = T2A(filename);
		cv::String Path(imgpath);
		I.src = cv::imread(Path);

		lbl.sourceImage.push_back(I);
	}

	for (int i = 0; i < N; i++)
	{
		std::string str, subs;
		USES_CONVERSION;//������ʶ��
		str = T2A(lbl.sourceImage[i].name);

		subs = str.substr(0, str.find_first_of("."));

		str = translate_mat_dir + subs + ".txt";

		if (-1 == LoadData(str, in.transfMat[i], 4, 4, 1))
		{
			assert("ERROR: Read transform file"); exit(-1);
		}
	}

	return lbl;
}

int LoadData(std::string fileName, cv::Mat& matData, int matRows, int matCols, int matChns)
{
	int retVal = 0;

	// ���ļ�
	std::ifstream inFile(fileName.c_str(), std::ios_base::in);
	if (!inFile.is_open())
	{
		std::cout << "��ȡ�ļ�ʧ��" << std::endl;
		retVal = -1;
		return (retVal);
	}

	// ��������  
	std::istream_iterator<float> begin(inFile);    //�� float ��ʽȡ�ļ�����������ʼָ��  
	std::istream_iterator<float> end;          //ȡ�ļ�������ֹλ��  
	std::vector<float> inData(begin, end);      //���ļ����ݱ����� std::vector ��  
	cv::Mat tmpMat = cv::Mat(inData);       //�������� std::vector ת��Ϊ cv::Mat  

	// ����������д���  
	//copy(vec.begin(),vec.end(),ostream_iterator<double>(cout,"\t"));   

	// ����趨�ľ���ߴ��ͨ����  
	size_t dataLength = inData.size();
	//1.ͨ����  
	if (matChns == 0)
	{
		matChns = 1;
	}
	//2.������  
	if (matRows != 0 && matCols == 0)
	{
		matCols = dataLength / matChns / matRows;
	}
	else if (matCols != 0 && matRows == 0)
	{
		matRows = dataLength / matChns / matCols;
	}
	else if (matCols == 0 && matRows == 0)
	{
		matRows = dataLength / matChns;
		matCols = 1;
	}
	//3.�����ܳ���  
	if (dataLength != (matRows * matCols * matChns))
	{
		std::cout << "��������ݳ��� ������ �趨�ľ���ߴ���ͨ����Ҫ�󣬽���Ĭ�Ϸ�ʽ�������" << std::endl;
		retVal = 1;
		matChns = 1;
		matRows = dataLength;
	}

	// ���ļ����ݱ������������  
	matData = tmpMat.reshape(matChns, matRows).clone();

	return (retVal);
}



void createGaussianPyramid()
{	
	for (int j = 0; j < N; j++)
	{
		TextureImage img = in.label.sourceImage[j];
		in.pyramids[j][0] = img;
		for (int i = 1; i < FLOOR_NUM; i++)
		{
			cv::pyrDown(img.src, img.src, cv::Size(img.src.cols / 2, img.src.rows / 2));
			in.pyramids[j][i] = img;
		}
	}
}
//Recursively create a 6-floor-image-pyramid for input image.
void CreateGaussianPyramid(TextureImage img, std::vector<TextureImage> &Pyramid)
{
	if (in.numFloors == 0)
	{
		Pyramid.push_back(img);
	}
	cv::Mat temp;
	TextureImage tmp;
	pyrDown(img.src, temp, cv::Size(img.src.cols / 2, img.src.rows / 2));

	tmp.src = temp;
	tmp.name = img.name;

	Pyramid.push_back(tmp);
	in.numFloors++;
	if (in.numFloors == 5)
	{
		in.numFloors = 0;
		return;
	}
	else
		CreateGaussianPyramid(tmp, Pyramid);
}

int init()
{
	InitiateGLFW();
	InitiateGLEW();
	InitiateData();

	glViewport(0, 0, screenWidth, screenHeight);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Setup some OpenGL options
	glDepthFunc(GL_LEQUAL);

	const unsigned char* version = glGetString(GL_VERSION);
	std::cout << version << std::endl;
	std::cout << glGetString(GL_VENDOR) << std::endl;
	const GLubyte *renderer = glGetString(GL_RENDERER);
	printf("GL Renderer : %s\n", renderer);

	return EXIT_SUCCESS;
}