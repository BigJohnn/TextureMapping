#include "stdafx.h"

#include "GraphProcess.h"
//#include "opencl.h"

extern InputData in;
#define CP 36
bool processface = true;

//Record the translation positions of each image.
pair<int, int> t_poses[CP];

glm::vec2 operator*(const glm::vec2 &v, const double &m) {
	return glm::vec2(v.x*m, v.y*m);
}

point3 operator-(const point3 &a, const point3 &b)
{
	return point3(a.get<0>() - b.get<0>(), a.get<1>() - b.get<1>(), a.get<2>() - b.get<2>());
}

int minimize_energy(TypeGeneral::REAL energy[], int size)
{
	struct node arry[CP];
	for (int j = 1; j <= CP; j++)
	{
		arry[j - 1].data = energy[j];
		arry[j - 1].order = j;
	}
	qsort(arry, 9, sizeof(struct node), cmp);

	return arry[0].order-1;
}
void next_entries2(int s, int t, int fOrder, int *entries, double(*esmooth)[N9])
{
	for (int i = 1; i <= N; i++)
	{
		struct node arry[9];
		for (int j = 1; j <= 9; j++)
		{
			arry[j - 1].data = (double)esmooth[s * 9 + t][i * 9 + j];
			arry[j - 1].order = j;
		}
		qsort(arry, 9, sizeof(struct node), cmp);

		for (int j = 0; j < 9; j++)
		{
			entries[fOrder * N + i - 1] = arry[j].order - 1;
		}
	}
}

double CalcuEdataSin22(triangle3 t3, glm::vec3 camPos)
{
	point3 a = t3.outer().at(0);
	point3 b = t3.outer().at(1);
	point3 c = t3.outer().at(2);

	//面元重心
	glm::vec3 facet_ctr((a.get<0>()+b.get<0>()+c.get<0>())/3,
		(a.get<1>() + b.get<1>() + c.get<1>()) / 3, 
		(a.get<2>() + b.get<2>() + c.get<2>()) / 3);
	
	glm::vec3 view_dir = facet_ctr - camPos;

	point3 m = t3.outer().at(1) - t3.outer().at(0);//AB
	point3 n = t3.outer().at(2) - t3.outer().at(0);//AC

	glm::vec3 facetnorm(m.get<1>()*n.get<2>() - m.get<2>()*n.get<1>(), m.get<2>()*n.get<0>() - m.get<0>()*n.get<2>(),
		m.get<0>()*n.get<1>() - m.get<1>()*n.get<0>());

	float cos2 = glm::dot(facetnorm, view_dir)*glm::dot(facetnorm, view_dir)
		/ (facetnorm.x*facetnorm.x + facetnorm.y*facetnorm.y + facetnorm.z*facetnorm.z)
		/ (view_dir.x*view_dir.x + view_dir.y*view_dir.y + view_dir.z*view_dir.z);
	
	double sin2 = 1 - cos2;

	//Very very important to display the correct textures.!
	double sign = glm::dot(facetnorm, view_dir) / sqrt(facetnorm.x*facetnorm.x + facetnorm.y*facetnorm.y + facetnorm.z*facetnorm.z)
		/ sqrt(view_dir.x*view_dir.x + view_dir.y*view_dir.y + view_dir.z*view_dir.z) / sqrt(sin2);

	return sign*sin2;
}

//当前面元与当前输入图像的夹角余弦值的平方-Edata
double CalcuEdataSin2(triangle3 t3, glm::vec3 imageNorm)
{
	point3 a = t3.outer().at(1) - t3.outer().at(0);//AB
	point3 b = t3.outer().at(2) - t3.outer().at(0);//AC
	
	glm::vec3 facetnorm(a.get<1>()*b.get<2>() - a.get<2>()*b.get<1>(), a.get<2>()*b.get<0>() - a.get<0>()*b.get<2>(),
		a.get<0>()*b.get<1>() - a.get<1>()*b.get<0>());

	float cos2 = glm::dot(facetnorm, imageNorm)*glm::dot(facetnorm, imageNorm)
		/ (facetnorm.x*facetnorm.x + facetnorm.y*facetnorm.y + facetnorm.z*facetnorm.z)
		/ (imageNorm.x*imageNorm.x + imageNorm.y*imageNorm.y + imageNorm.z*imageNorm.z);
	double sin2 = 1 - cos2;
	return sin2;
}

double calculateEsmooth0(halfedge *he, int pI, int pI1, triangle2 t2, triangle2 t2adj, segment2 edge)
{
	bg::correct(t2);
	bg::correct(t2adj);
	EdgeInfoSmooth edgeinfo;
	edgeinfo.Esmooth = 0;
	edgeinfo.edge = edge;
	edgeinfo.texture0 = in.pyramids[pI][0].src;
	edgeinfo.texture1 = in.pyramids[pI1][0].src;

	double x0 = edge.first.get<0>();
	double y0 = edge.first.get<1>();
	double x1 = edge.second.get<0>();
	double y1 = edge.second.get<1>();
	LineDDA(x0, y0, x1, y1, (LINEDDAPROC)EsmoothOnEdge0, (LPARAM)(LPVOID)&edgeinfo);

	double length = std::sqrt((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1));
	edgeinfo.Esmooth /= length;
	return edgeinfo.Esmooth;
}
double calculateEsmooth(halfedge *he, int pI, int pI1, int left, int right, triangle2 t2, triangle2 t2adj, segment2 edge, int floor)
{
	bg::correct(t2);
	bg::correct(t2adj);
	EdgeInfoSmooth edgeinfo;
	edgeinfo.Esmooth = 0;
	edgeinfo.edge = edge;
	edgeinfo.texture0 = in.pyramids[pI][floor].src;
	edgeinfo.texture1 = in.pyramids[pI1][floor].src;
	edgeinfo.left = left;
	edgeinfo.right = right;

	
	int x0 = (int)edge.first.get<0>() >> floor;
	int y0 = (int)edge.first.get<1>() >> floor;
	int x1 = (int)edge.second.get<0>() >> floor;
	int y1 = (int)edge.second.get<1>() >> floor;
	LineDDA(x0, y0, x1, y1, (LINEDDAPROC)EsmoothOnEdge, (LPARAM)(LPVOID)&edgeinfo);

	double length = std::sqrt((x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1));
	edgeinfo.Esmooth /= length;
	return edgeinfo.Esmooth;
}

std::pair<cv::Mat, cv::Mat> calculateRTvec(cv::Mat modelMat, cv::Mat imageMat)
{
	cv::Mat RT = modelMat.inv()*imageMat.inv();

	//cv::Mat RT = modelMat *modelMat /  imageMat;
	cv::Mat r(RT, cv::Rect(0, 0, 3, 3));
	cv::Mat T(RT, cv::Rect(3, 0, 1, 3));

	cv::Mat R;///ROTATE VEC 3X1

	cv::Rodrigues(r, R);

	//cout << R <<endl<< r<<endl << RT << endl;
	return std::pair<cv::Mat, cv::Mat>(R, T);
}

std::pair<int, int> NearestCamIsTheInput(Mesh *mesh, halfedge *he, std::vector<triangle3> tcp)
{
	std::pair<int, int> p;
	p.second = 4;

	segment3 edge3(he->getOrigin()->getData().position, he->getTwin()->getOrigin()->getData().position);
	triangle3 t3 = tcp[0];
	
	glm::vec3 BA(t3.outer().at(1).get<0>() - t3.outer().at(0).get<0>(),
		t3.outer().at(1).get<1>() - t3.outer().at(0).get<1>(),
		t3.outer().at(1).get<2>() - t3.outer().at(0).get<2>());
	glm::vec3 CB(t3.outer().at(2).get<0>() - t3.outer().at(1).get<0>(),
		t3.outer().at(2).get<1>() - t3.outer().at(1).get<1>(),
		t3.outer().at(2).get<2>() - t3.outer().at(1).get<2>());
	glm::vec3 face_norm = glm::cross(BA, CB);
	point3 gC;//geometry center.
	
	gC = point3((t3.outer().at(0).get<0>() + t3.outer().at(1).get<0>() + t3.outer().at(2).get<0>()) / 2,
			(t3.outer().at(0).get<1>() + t3.outer().at(1).get<1>() + t3.outer().at(2).get<1>()) / 2,
			(t3.outer().at(0).get<2>() + t3.outer().at(1).get<2>() + t3.outer().at(2).get<2>()) / 2);
	
	point3 camPos[N];
	double dist[N];
	cv::Mat cam_pos[N];
	for (int i = 0; i < N; i++)
	{
		cv::Mat RT = in.transfMat[i] * in.modelMatrix;

		cv::Mat r(RT, cv::Rect(0, 0, 3, 3));

		//cv::Mat T(RT, cv::Rect(3, 0, 1, 3));
		cv::Mat T(RT, cv::Rect(3, 0, 1, 4));
		cv::Mat r_inv = r.inv(); //cout << r << r_inv << endl;

		//像平面法向量
		cv::Mat img_norm(r_inv, cv::Rect(2, 0, 1, 3)); //可以  因z轴坐标（0,0,1）

		glm::vec3 imgNorm(img_norm.at<float>(0, 0), img_norm.at<float>(1, 0), img_norm.at<float>(2, 0));
		cam_pos[i] = T;
		camPos[i] = point3((double)cam_pos[i].at<float>(0, 0), 
			(double)cam_pos[i].at<float>(1, 0),
			(double)cam_pos[i].at<float>(2, 0));
		if (glm::dot(face_norm, imgNorm) > 0)
		{
			dist[i] = MAXDWORD;
		}
		else
		{
			dist[i] = abs(bg::distance(gC, camPos[i]));
		}
	}
	
	double min_dist;
	min_dist = min(min(dist[0], dist[1]), min(dist[2], dist[3]));
	for (int i = 0; i < N; i++)
	{
		if (min_dist == dist[i])
		{
			p.first = i;
		}
	}
	return p;
}

aiVector3D normalize(aiVector3D &pos)
{
	pos.x /= in.label.sourceImage[0].src.cols;
	pos.y /= in.label.sourceImage[0].src.rows;
	pos.x -= 0.042;
	pos.y += 0.022;

	//pos.x += 0.042;
	//pos.y -= 0.022;
	return pos;
}

void process0(Mesh *mesh, int fOrder/*面元编号*/, halfedge *he/*当前处理的半边*/, MRFEnergy<TypeGeneral>* mrf,
	MRFEnergy<TypeGeneral>::NodeId* nodes, Object4Process target = normal)
{
	std::vector<triangle3> tcp;
	triangle3 t3 = CreateTriangle3(he->getFace());
	tcp.push_back(t3);

	if (target != single_face)
	{
		triangle3 t3_twin = CreateTriangle3(he->getTwin()->getFace());
		tcp.push_back(t3_twin);
	}
	
	segment3 edge3(he->getOrigin()->getData().position, he->getTwin()->getOrigin()->getData().position);

	const int K = N; // number of labels
	TypeGeneral::REAL V[K*K];
	memset(V, 0, sizeof(TypeGeneral::REAL) * K * K);
	TypeGeneral::REAL Dx[K], Dy[K];
	memset(Dx, 0, sizeof(TypeGeneral::REAL) * N);
	memset(Dy, 0, sizeof(TypeGeneral::REAL) * N);
	//pyramidIndex pI
	for (int pI = 0; pI < N; pI++)
	{
		std::pair<cv::Mat, cv::Mat> RTvec =
			calculateRTvec(in.modelMatrix, in.transfMat[pI]);

		//物-像 变换(视口变换)
		triangle2 t2 = model3D_ImageT2D(tcp[0], RTvec.first, RTvec.second);
		segment2 edge2 = seg3_seg2(edge3, RTvec.first, RTvec.second);

		mesh->viewMatrices[pI] = in.transfMat[pI];

		cv::Mat RT = in.transfMat[pI];// .inv()*in.modelMatrix;
		cv::Mat r(RT, cv::Rect(0, 0, 3, 3));

		cv::Mat T(RT, cv::Rect(3, 0, 1, 4));

		glm::vec3 camPos(T.at<float>(0, 0), T.at<float>(1, 0),
			T.at<float>(2, 0));

		cv::Mat r_inv = r.inv(); //cout << r << r_inv << endl;

		//像平面法向量
		cv::Mat img_norm(r_inv, cv::Rect(2, 0, 1, 3)); //可以  因z轴坐标（0,0,1）

		glm::vec3 imageNorm(img_norm.at<float>(0, 0), img_norm.at<float>(1, 0), 
			img_norm.at<float>(2, 0));

		Dx[pI] = CalcuEdataSin22(t3, camPos);
		//Dx[pI] = CalcuEdataSin2(t3, imageNorm);
		nodes[fOrder] = mrf->AddNode(TypeGeneral::LocalSize(K), TypeGeneral::NodeData(Dx));

		if (tcp.size() == 2)//输入非单个三角形 的情形。
		{
			triangle2 t2adj = model3D_ImageT2D(tcp[1], RTvec.first, RTvec.second);

			Dy[pI] = CalcuEdataSin22(tcp[1], camPos);
			//Dy[pI] = CalcuEdataSin2(tcp[1], imageNorm);

			nodes[mesh->getFaceId(he->getTwin()->getFace())] = mrf->AddNode(TypeGeneral::LocalSize(K), TypeGeneral::NodeData(Dy));

			for (int pI1 = pI + 1; pI1 < N; pI1++)
			{
				V[pI1 + pI*K] = V[pI + pI1*K] = calculateEsmooth0(he, pI, pI1, t2, t2adj, edge2);
			}
		}

	}//endfor pI

	//若三角形成对出现，则用边结点来刻画纹理接缝
	if (tcp.size() == 2)
		mrf->AddEdge(nodes[fOrder], nodes[mesh->getFaceId(he->getTwin()->getFace())], TypeGeneral::EdgeData(TypeGeneral::GENERAL, V));
}
void process(Mesh *mesh, int fOrder/*面元编号*/, MRFEnergy<TypeGeneral>* mrf[],
	MRFEnergy<TypeGeneral>::NodeId* nodes[], int floor, Object4Process target = normal)
{
	halfedge *he = mesh->getFace(fOrder)->getBoundary();

	std::vector<triangle3> tcp;
	triangle3 t3 = CreateTriangle3(he->getFace());
	tcp.push_back(t3);

	if (target != single_face)
	{
		triangle3 t3_twin = CreateTriangle3(he->getTwin()->getFace());
		tcp.push_back(t3_twin);
	}

	segment3 edge3(he->getOrigin()->getData().position, he->getTwin()->getOrigin()->getData().position);

	const int K = N; // number of labels
	TypeGeneral::REAL V[CP][K*K];
	memset(V, 0, sizeof(TypeGeneral::REAL) * K * K * CP);
	TypeGeneral::REAL Dx[K], Dy[K];
	memset(Dx, 0, sizeof(TypeGeneral::REAL) * N);
	memset(Dy, 0, sizeof(TypeGeneral::REAL) * N);
	//pyramidIndex pI
	for (int pI = 0; pI < N; pI++)
	{
		std::pair<cv::Mat, cv::Mat> RTvec =
			calculateRTvec(in.modelMatrix, in.transfMat[pI]);

		//物-像 变换(视口变换)
		triangle2 t2 = model3D_ImageT2D(tcp[0], RTvec.first, RTvec.second);
		segment2 edge2 = seg3_seg2(edge3, RTvec.first, RTvec.second);

		mesh->viewMatrices[pI] = in.transfMat[pI];

		cv::Mat RT = in.transfMat[pI];// .inv()*in.modelMatrix;
		cv::Mat r(RT, cv::Rect(0, 0, 3, 3));

		cv::Mat T(RT, cv::Rect(3, 0, 1, 4));

		glm::vec3 camPos(T.at<float>(0, 0), T.at<float>(1, 0),
			T.at<float>(2, 0));

		cv::Mat r_inv = r.inv(); //cout << r << r_inv << endl;

		//像平面法向量
		cv::Mat img_norm(r_inv, cv::Rect(2, 0, 1, 3)); //可以  因z轴坐标（0,0,1）

		glm::vec3 imageNorm(img_norm.at<float>(0, 0), img_norm.at<float>(1, 0),
			img_norm.at<float>(2, 0));

		Dx[pI] = CalcuEdataSin22(t3, camPos);
		//Dx[pI] = CalcuEdataSin2(t3, imageNorm);

		int k = 0;
		for (int left = 0; left < 9; left++)
		{
			for (int right = left + 1; right < 9; right++)
			{
				nodes[k][fOrder] = mrf[k]->AddNode(TypeGeneral::LocalSize(K), TypeGeneral::NodeData(Dx));

				if (tcp.size() == 2)//输入非单个三角形 的情形。
				{
					triangle2 t2adj = model3D_ImageT2D(tcp[1], RTvec.first, RTvec.second);

					Dy[pI] = CalcuEdataSin22(tcp[1], camPos);
					//Dy[pI] = CalcuEdataSin2(tcp[1], imageNorm);

					nodes[k][mesh->getFaceId(he->getTwin()->getFace())] = mrf[k]->AddNode(TypeGeneral::LocalSize(K), TypeGeneral::NodeData(Dy));

					for (int pI1 = pI + 1; pI1 < N; pI1++)
					{
						V[k][pI1 + pI*K] = V[k][pI + pI1*K] = calculateEsmooth(he, pI, pI1, left, right, t2, t2adj, edge2, floor);
					}
				}
				t_poses[k].first = left;
				t_poses[k].second = right;
				k++;
			}
			
		}
		

	}//endfor pI

	//若三角形成对出现，则用边结点来刻画纹理接缝
	if (tcp.size() == 2)
	{
		for (int i = 0; i < CP; i++)
		{
			mrf[i]->AddEdge(nodes[i][fOrder], nodes[i][mesh->getFaceId(he->getTwin()->getFace())], TypeGeneral::EdgeData(TypeGeneral::GENERAL, V[i]));
		}
	}
		
}

std::vector<Texture> FaceTraverse0(Mesh* mesh)
{
	for (unsigned int a = 0; a < N; a++)
	{
		mesh->mTextureCoords[a] = new aiVector3D[mesh->getVertices().size()];
		for (int i = 0; i < mesh->getVertices().size(); i++)
		{
			mesh->mTextureCoords[a][i] = aiVector3D(0, 0, 0);
		}
	}

	MRFEnergy<TypeGeneral>* mrf;
	MRFEnergy<TypeGeneral>::NodeId* nodes;
	MRFEnergy<TypeGeneral>::Options options;
	TypeGeneral::REAL energy, lowerBound;

	const int nodeNum = mesh->getNumFaces(); // number of nodes
	
	int *x = new int[nodeNum];//solution array

	mrf = new MRFEnergy<TypeGeneral>(TypeGeneral::GlobalSize());
	nodes = new MRFEnergy<TypeGeneral>::NodeId[nodeNum];

	double start = glfwGetTime();
	using namespace boost;
	progress_display p_d(mesh->getNumFaces());

	for (int i = 0; i < mesh->getNumFaces(); i++)
	{
		halfedge *first = mesh->getFace(i)->getBoundary();
		halfedge *last = nullptr;
		bool flag = false;
		while (first != last)
		{
			if (flag == false)
			{
				last = first;
				flag = true;
			}
			
			if (!last->getData().isModified())
			{
				if (last->getTwin()->getFace() == NULL)
				{
					process0(mesh, i, last, mrf, nodes, single_face);
				}
				else
				{
					process0(mesh, i, last, mrf, nodes);
				}
				last->getData().modify();
				last->getTwin()->getData().modify();
			}
			last = last->getNext();
		}
		++p_d;
	}

	cout << "spend" << glfwGetTime() - start << "s" << endl;

	//mrf->SetAutomaticOrdering();

	options.m_iterMax = 30; // maximum number of iterations
	mrf->Minimize_TRW_S(options, lowerBound, energy);

	for (int i = 0; i < mesh->getNumFaces(); i++)
	{
		x[i] = mrf->GetSolution(nodes[i]);
		
		//忽略t，解即s
		int s = x[i];

		Texture texture;

		bool IsNewTexture = true;
		for (int i = 0; i < global_texture_loaded.size(); i++)
		{
			if (global_texture_loaded[i] == s) IsNewTexture = false;
		}

		if (IsNewTexture)
		{
			texture.id_gl = TextureFromFile(in.label.sourceImage[s].src);
			texture.id = s;
			texture.type = "texture_diffuse";
			global_textures.push_back(texture);
			global_texture_loaded.push_back(s);
		}

		std::pair<cv::Mat, cv::Mat> RTvec =
			calculateRTvec(in.modelMatrix, in.transfMat[s]);

		halfedge* he = mesh->getFace(i)->getBoundary();
		triangle3 t3 = CreateTriangle3(he->getFace());
		triangle2 t2 = model3D_ImageT2D(t3, RTvec.first, RTvec.second);

		aiVector3D posA(t2.outer().at(0).get<0>(),
			t2.outer().at(0).get<1>(), 0.0f);
		aiVector3D posB(t2.outer().at(1).get<0>(),
			t2.outer().at(1).get<1>(), 0.0f);
		aiVector3D posC(t2.outer().at(2).get<0>(),
			t2.outer().at(2).get<1>(), 0.0f);

		FaceData fdata = he->getFace()->getData();

		for (int c = 0; c < 3; c++)
		{
			mesh->indices4draw[s].push_back(fdata.indices[c]);
		}

		/*
		*注意！ 这里因为KScan3D的关系，不同的模型需要选择不同的  顶点顺序！！
		*/
		/*mesh->mTextureCoords[STtuple.first][fdata.indices[0]] = normalize(posB);
		mesh->mTextureCoords[STtuple.first][fdata.indices[1]] = normalize(posC);
		mesh->mTextureCoords[STtuple.first][fdata.indices[2]] = normalize(posA);*/

		mesh->mTextureCoords[s][fdata.indices[0]] = normalize(posB);
		mesh->mTextureCoords[s][fdata.indices[1]] = normalize(posA);
		mesh->mTextureCoords[s][fdata.indices[2]] = normalize(posC);

		
		//在进行下一层次计算之前清空MRF信息
		mesh->getFace(i)->getBoundary()->getData().antimodify();
		mesh->getFace(i)->getBoundary()->getTwin()->getData().antimodify();
	}
	delete[] x;
	delete nodes;
	delete mrf;

	return global_textures;
}

void pyramid_Optimization(Mesh* mesh, int nodeNum, int floor, int &min, MRFEnergy<TypeGeneral>* mrf[], MRFEnergy<TypeGeneral>::NodeId* nodes[])
{
	MRFEnergy<TypeGeneral>::Options options;
	TypeGeneral::REAL energy[CP], lowerBound;

	/*for (int i = 0; i < CP; i++)
	{
		if (!nodes[i])
			nodes[i] = new MRFEnergy<TypeGeneral>::NodeId[nodeNum];
		if (!mrf[i])
			mrf[i] = new MRFEnergy<TypeGeneral>(TypeGeneral::GlobalSize());
	}*/

	double start = glfwGetTime();
	using namespace boost;
	progress_display p_d(mesh->getNumFaces());

	for (int i = 0; i < mesh->getNumFaces(); i++)
	{
		/*halfedge *first = mesh->getFace(i)->getBoundary();
		while (first != mesh->getFace(i)->getBoundary()->getNext())
		{
			cout << "1" << endl;
		}*/
		if (!mesh->getFace(i)->getBoundary()->getData().isModified())
		{
			if (mesh->getFace(i)->getBoundary()->getTwin()->getFace() == NULL)
			{
				process(mesh, i, mrf, nodes, floor, single_face);
			}
			else
			{
				process(mesh, i, mrf, nodes, floor);
			}
			mesh->getFace(i)->getBoundary()->getData().modify();
			mesh->getFace(i)->getBoundary()->getTwin()->getData().modify();
		}

		++p_d;
	}

	cout << "spend" << glfwGetTime() - start << "s" << endl;

	//mrf->SetAutomaticOrdering();

	options.m_iterMax = 30; // maximum number of iterations

	for (int i = 0; i < CP; i++)
		mrf[i]->Minimize_TRW_S(options, lowerBound, energy[i]);

	min = minimize_energy(energy, CP);

	if (floor != 0)
		for (int i = 0; i < N; i++)
		{
			translationTransform(in.pyramids[i][floor-1].src, in.pyramids[i][floor-1].src, glm::vec2(t_poses[min].first, t_poses[min].second)*2);
		}
}

std::vector<Texture> FaceTraverse(Mesh* mesh)
{
	for (unsigned int a = 0; a < N; a++)
	{
		mesh->mTextureCoords[a] = new aiVector3D[mesh->getVertices().size()];
		for (int i = 0; i < mesh->getVertices().size(); i++)
		{
			mesh->mTextureCoords[a][i] = aiVector3D(0, 0, 0);
		}
	}

	int k = 0;
	for (int i = -1; i < 1; i++)
	{
		for (int j = -1; j < 1; j++)
		{
			translation[k++] = pair<int, int>(i, j);
		}
	}

	const int nodeNum = mesh->getNumFaces(); // number of nodes

	MRFEnergy<TypeGeneral>* mrf[CP];//9种平移 两两组合为CP-36种mrf
	MRFEnergy<TypeGeneral>::NodeId* nodes[CP];
	
	for (int i = 0; i < CP; i++)
	{
		mrf[i] = new MRFEnergy<TypeGeneral>(TypeGeneral::GlobalSize());
		nodes[i] = new MRFEnergy<TypeGeneral>::NodeId[nodeNum];
	}

	int min = 0;
	for (int f = 5; f >=0; f--)
	{
		pyramid_Optimization(mesh,nodeNum,f,min,mrf,nodes);
	}

	//translationTransform(cv::Mat &src, cv::Mat& dst, glm::vec2(t_poses[min].first,t_poses[min].second));

	int *x = new int[nodeNum];//solution array
	for (int i = 0; i < mesh->getNumFaces(); i++)
	{
		x[i] = mrf[min]->GetSolution(nodes[min][i]);

			
		int s = x[i];

		Texture texture;

		bool IsNewTexture = true;
		for (int i = 0; i < global_texture_loaded.size(); i++)
		{
			if (global_texture_loaded[i] == s) IsNewTexture = false;
		}

		if (IsNewTexture)
		{
			texture.id_gl = TextureFromFile(in.label.sourceImage[s].src);
			texture.id = s;
			texture.type = "texture_diffuse";
			global_textures.push_back(texture);
			global_texture_loaded.push_back(s);
		}

		std::pair<cv::Mat, cv::Mat> RTvec =
			calculateRTvec(in.modelMatrix, in.transfMat[s]);

		halfedge* he = mesh->getFace(i)->getBoundary();
		triangle3 t3 = CreateTriangle3(he->getFace());
		triangle2 t2 = model3D_ImageT2D(t3, RTvec.first, RTvec.second);

		aiVector3D posA(t2.outer().at(0).get<0>(),
			t2.outer().at(0).get<1>(), 0.0f);
		aiVector3D posB(t2.outer().at(1).get<0>(),
			t2.outer().at(1).get<1>(), 0.0f);
		aiVector3D posC(t2.outer().at(2).get<0>(),
			t2.outer().at(2).get<1>(), 0.0f);

		FaceData fdata = he->getFace()->getData();

		for (int c = 0; c < 3; c++)
		{
			mesh->indices4draw[s].push_back(fdata.indices[c]);
		}

		/*
		*注意！ 这里因为KScan3D的关系，不同的模型需要选择不同的  顶点顺序！！
		*/
		/*mesh->mTextureCoords[STtuple.first][fdata.indices[0]] = normalize(posB);
		mesh->mTextureCoords[STtuple.first][fdata.indices[1]] = normalize(posC);
		mesh->mTextureCoords[STtuple.first][fdata.indices[2]] = normalize(posA);*/

		mesh->mTextureCoords[s][fdata.indices[0]] = normalize(posB);
		mesh->mTextureCoords[s][fdata.indices[1]] = normalize(posA);
		mesh->mTextureCoords[s][fdata.indices[2]] = normalize(posC);


		//在进行下一层次计算之前清空MRF信息
		mesh->getFace(i)->getBoundary()->getData().antimodify();
		mesh->getFace(i)->getBoundary()->getTwin()->getData().antimodify();
	}
	
	
	for (int i = 0; i < CP; i++)
	{
		delete nodes[i];
		delete mrf[i];
	}

	delete[] x;
	

	return global_textures;
}