#include "stdafx.h"
#include "TRW_S.h"

//Minimize function E(x, y) = Dx(x) + Dy(y) + lambda*[x != y] + Dz(z) + V(y, z) where
//x, y \in{ 0, 1, 2 }, z \in{ 0, 1 }
//Dx(0) = 0, Dx(1) = 1, Dx(2) = 2,
//Dy(0) = 3, Dy(1) = 4, Dy(2) = 5,
//lambda = 6,
//[.] is 1 if it's argument is true, and 0 otherwise.
//Dz(0) = 7, Dz(1) = 8,
//V(y, z) = y*y + z

int TRW_S(double Edata[][9], double Esmooth[][N9])
{
	MRFEnergy<TypeGeneral>* mrf;
	MRFEnergy<TypeGeneral>::NodeId* nodes;
	MRFEnergy<TypeGeneral>::Options options;
	TypeGeneral::REAL energy, lowerBound;

	const int nodeNum = 2; // number of nodes
	const int K = N9; // number of labels
	TypeGeneral::REAL Dx[K];
	TypeGeneral::REAL Dy[K];
	int x;
	TypeGeneral::REAL lamda = 1;
	TypeGeneral::REAL V[K*K];

	mrf = new MRFEnergy<TypeGeneral>(TypeGeneral::GlobalSize());
	nodes = new MRFEnergy<TypeGeneral>::NodeId[nodeNum];

	// construct energy
	int k = 0;

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			Dx[k++] = Edata[i][j];
		}
	}

	nodes[0] = mrf->AddNode(TypeGeneral::LocalSize(K), TypeGeneral::NodeData(Dx));
	for (int i = 0; i < K; i++)
	{
		Dy[i] = 0;//9*i+4 is the source image with a  translation vector (0,0).
	}
	nodes[1] = mrf->AddNode(TypeGeneral::LocalSize(K), TypeGeneral::NodeData(Dy));
	//mrf->AddEdge(nodes[0], nodes[1], TypeGeneral::EdgeData(lamda));

#pragma omp parallel for
	for (int i = 0; i < K; i++)
	{
		for (int j = 0; j < K; j++)
		{
			V[j + i*K] = Esmooth[i][j] * lamda;//lamda*sum_of_Esmooth[i] = sum of lamda*Esmooth[i], i belongs to 0~K
		}
	}
	mrf->AddEdge(nodes[0], nodes[1], TypeGeneral::EdgeData(TypeGeneral::GENERAL, V));
	// Function below is optional - it may help if, for example, nodes are added in a random order
	// mrf->SetAutomaticOrdering();

	/////////////////////// TRW-S algorithm //////////////////////
	options.m_iterMax = 30; // maximum number of iterations
	mrf->Minimize_TRW_S(options, lowerBound, energy);

	// read solution
	x = mrf->GetSolution(nodes[0]);

	//printf("Solution: %d\n", x);

	// done
	delete nodes;
	delete mrf;
	return x;
}