#include "CardiacMesh.h"
#include <cmath>
#include <algorithm>
#include <set>
#include <matio.h>
#include <stdlib.h>
#include <baseTSD.h>
#include <fstream>


#define ELECTR_DEPTH 50
 #define CALCULATE_ELECTR_TOTAL 1
CardiacMesh::CardiacMesh()
{
	maximumCV = 1; // [cm/ms]
	maxDiffusion = 0.5;
	minDiffusion = 0.0;
	diffusion_CV20 = 0.0003;
	maxERP = 300;
	minERP = 10;
	maxExcitability = 20;
	minExcitability= 0.1;
	maxElectrogram = -1999;
	minElectrogram = 999;
	stimulationBegun = false;

	_minRefractoryTreshold = 50.0; //[ms]
	_derivativeTreshold = 0.05;
	m_simulationTime = 0;
	structureUpdated = false;
	m_vertexMatrix = nullptr;
	m_indicesMatrix = nullptr;
}


CardiacMesh::~CardiacMesh()
{
	free(m_indicesMatrix);
	free(m_vertexMatrix);
	this->destroyGrid();
}

CardiacMesh* CardiacMesh::constructCartesianGridPlain(int x, int y, double dx, double dy, CELL_TYPE type)
{
	CardiacMesh *grid = new CardiacMesh();
	int totalSize = y*x;
	for (int j = 0; j < totalSize; ++j)
	{
		grid->addNode(j, static_cast<double>(j % x) * dx, dy * floor(static_cast<double>(j) / static_cast<double>(x)), 0, ATRIAL_V3);
	}
	constructCartesianGrid(x, y, dx, dy, grid);
	return grid;

}
CardiacMesh* CardiacMesh::constructCartesianGridRightAtrium(int x, int y, double dx, double dy, double percentageOfWidth)
{
	CardiacMesh *grid = new CardiacMesh();
	int totalSize = y*x;

	for (int j = 0; j < totalSize; ++j)
	{
		if (static_cast<double>(j % x) < (x *percentageOfWidth) && j < totalSize *percentageOfWidth)
			grid->addNode(j, static_cast<double>(j % x) * dx, dy * floor(static_cast<double>(j) / static_cast<double>(x)), 0, SA_NODE);
		else if (static_cast<double>(j % x) >(x - x *percentageOfWidth) && j > totalSize - totalSize *percentageOfWidth)
			grid->addNode(j, static_cast<double>(j % x) * dx, dy * floor(static_cast<double>(j) / static_cast<double>(x)), 0, AV_NODE);
		else
			grid->addNode(j, static_cast<double>(j % x) * dx, dy * floor(static_cast<double>(j) / static_cast<double>(x)), 0, ATRIAL_V3);
	}
	constructCartesianGrid(x, y, dx, dy, grid);
	return grid;
}

CardiacMesh* CardiacMesh::constructCartesianGridBlockTest(int x, int y, double dx, double dy)
{
	CardiacMesh *grid = new CardiacMesh();
	int totalSize = y*x;
	for (int j = 0; j < totalSize; ++j)
	{
		grid->addNode(j, static_cast<double>(j % x) * dx, dy * floor(static_cast<double>(j) / static_cast<double>(x)), 0, ATRIAL_V3);
	}
	constructCartesianGrid(x, y, dx, dy, grid);


	int gridSize = grid->m_mesh.size();
	double sigg = grid->diffusion_CV20;



	for (int j = 0; j < gridSize; ++j)
	{
		grid->m_mesh[j]->setSigma(sigg, sigg, 0);
		if ((j % x) == (x / 2))
		{
			grid->m_mesh[j]->m_connexin[1] = 0;
			grid->m_mesh[j]->m_ConnexinSum = 3 * grid->m_mesh[j]->m_ConnexinSum / 4;

		}
		if ((j % x) == (x / 2) )
		{
			//grid->m_mesh[j]->m_connexin[1] = 0;
			//	grid->m_mesh[j]->m_connexin[0] = 0;
			//	grid->m_mesh[j]->m_connexin[2] = 0;
			//	grid->m_mesh[j]->m_connexin[3] = 0;
			//grid->m_mesh[j]->m_ConnexinSum = 3 * grid->m_mesh[j]->m_ConnexinSum / 4;
			//	grid->m_mesh[j]->m_ConnexinSum = 0;
			
		}

		if ((j % x) == (x / 2)+1)
		{
			grid->m_mesh[j]->m_connexin[0] = 0;
			grid->m_mesh[j]->m_ConnexinSum = 3 * grid->m_mesh[j]->m_ConnexinSum / 4;

		}
		//SET UP GRID ELECTRODES
		if ((j % x) == (x / 2) + 1)
		{
			grid->m_mesh[j]->m_connexin[0] = 0;
			grid->m_mesh[j]->m_ConnexinSum = 3 * grid->m_mesh[j]->m_ConnexinSum / 4;

		}
	}

	grid->gridElectrode = new GridElectrode(5, 10, 75, 16, 16);
	for (int j = grid->gridElectrode->posX + grid->gridElectrode->posY*x; j < gridSize; j = j + x)
	{
		for (int k = j; k < j + grid->gridElectrode->_numberOfElectrodesX*grid->gridElectrode->_spacingXY; k = k + grid->gridElectrode->_spacingXY)
		{
			grid->gridElectrode->_oscMatrix.push_back(grid->m_mesh[k]->oscillatorID);
			std::vector<double> seriesH;
			grid->gridElectrode->m_historyElectrogram.push_back(seriesH);
			std::vector<double> seriesAT;
			grid->gridElectrode->m_historyActivationTimes.push_back(seriesAT);
			std::vector<double> seriesHE;
			grid->gridElectrode->m_historyAfterHilbert.push_back(seriesHE);
			std::vector<double> seriesT;
			grid->gridElectrode->m_historyTime.push_back(seriesT);
			std::vector<double> seriesP;
			grid->gridElectrode->m_historyPotential.push_back(seriesP);
			
		}
		if (grid->gridElectrode->_oscMatrix.size() >= (grid->gridElectrode->_numberOfElectrodesX * grid->gridElectrode->_numberOfElectrodesY))
		{
			break;
		}
	}

	return grid;
}


CardiacMesh* CardiacMesh::constructCartesianGrid(int x, int y, double dx, double dy, CardiacMesh *grid)
{
	int totalSize = y*x;


	grid->_minNodalSpacing = max(dx, dy);
	grid->_maxNodalSpacing = min(dx, dy);
	grid->_sizeInMm = sqrt(static_cast<double>(x*x + y*y))*dx;
	//XXXXX
	grid->maxPotential = grid->maxElectrogram;
	grid->minPotential = grid->minElectrogram;

	//ADD NEIGHBOURS
	int idx, idy;
	for (int j = 0; j < totalSize; ++j)
	{
		idx = j % x;
		idy = floor(static_cast<double>(j) / static_cast<double>(x));
		
		if (idx > 0 && idx < x-1 && idy > 0 && idy < y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
		}
		else if (idx == 0 && idy > 0 && idy < y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == x-1 && idy > 0 && idy < y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx > 0 && idx < x-1  && idy == 0 )
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx > 0 && idx < x-1  && idy == y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == 0 && idy == 0)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == x-1 && idy == y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == x-1 && idy == 0)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == 0 && idy == y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->m_type = SOLID_WALL;
		}

	}

	grid->setVertexTriangleList(false);
	grid->calculateCenter();
	grid->setWallCells();
	grid->setDiffusionCoefficients();
	return grid;
}
CardiacMesh* CardiacMesh::constructStl(std::string inname, CELL_TYPE type)
{
	CardiacMesh *grid = new CardiacMesh();

	grid->m_vertexList.clear();


	std::ifstream myFile( inname.c_str(), ios::in | ios::binary);
	char header_info[80] = "";
	char nTri[4];
	unsigned long nTriLong;

	//read 80 byte header
	if (myFile) 
	{
		myFile.read(header_info, 80);
	}
	//read 4-byte ulong
	if (myFile) 
	{
		myFile.read(nTri, 4);
		nTriLong = *((unsigned long*)nTri);
	}

	int totalSize = nTriLong;
	grid->_minNodalSpacing = 1;
	grid->_maxNodalSpacing = 1;
	Oscillator *node = NULL;
	int oscillatorIDerator = 0;
	int currentID = -1;
	double scaleFactor  = 3;

	//Fill vertex matrix and indices matrix for OpenGL handling
	grid->m_vertexMatrix = (SVertex *)malloc(sizeof(SVertex) * 1 * totalSize);

	//now read in all the triangles
	for (int i = 0; i < totalSize; i++)
	{
		char facet[50];
		double m_x, m_y, m_z;
		SVertex vertex[3];
		SVertex normalV;
		if (myFile) {

			//read one 50-byte triangle
			myFile.read(facet, 50);

			//read normal
			char f1[4] = { facet[0], facet[1], facet[2], facet[3] };
			char f2[4] = { facet[4], facet[5], facet[6], facet[7] };
			char f3[4] = { facet[8], facet[9], facet[10], facet[11] };
			normalV.x = *((float*)f1);
			normalV.y = *((float*)f2);
			normalV.z = *((float*)f3);

			//facet + 12 skips the triangle's unit normal
			int vertexNum = 0;
			for (int vertIt = 12; vertIt < 40; vertIt = vertIt + 12)
			{
				char f1[4] = { facet[vertIt + 0], facet[vertIt + 1], facet[vertIt + 2], facet[vertIt + 3] };
				char f2[4] = { facet[vertIt + 4], facet[vertIt + 5], facet[vertIt + 6], facet[vertIt + 7] };
				char f3[4] = { facet[vertIt + 8], facet[vertIt + 9], facet[vertIt + 10], facet[vertIt + 11] };

				vertex[vertexNum].x = *((float*)f1);
				vertex[vertexNum].y = *((float*)f2);
				vertex[vertexNum].z = *((float*)f3);
				vertex[vertexNum].x *= scaleFactor;
				vertex[vertexNum].y *= scaleFactor;
				vertex[vertexNum].z *= scaleFactor;
				++vertexNum;
			}

			//decide, if notde is present or not and add it
			VertexTriangle *tri = new VertexTriangle;
			int idis[3];
			for (int vertIt = 0; vertIt < 3; vertIt++)
			{
				currentID = -1;
				for (int j = 0; j < grid->m_mesh.size(); ++j)
				{
					if (abs(grid->m_mesh[j]->m_x - vertex[vertIt].x) < 0.001 &&
						abs(grid->m_mesh[j]->m_y - vertex[vertIt].y) < 0.001 &&
						abs(grid->m_mesh[j]->m_z - vertex[vertIt].z) < 0.001)
					{
						currentID = grid->m_mesh[j]->oscillatorID;
						break;
					}
				}
				if (currentID == -1)
				{
					grid->addNode(oscillatorIDerator, vertex[vertIt].x, vertex[vertIt].y, vertex[vertIt].z, type);

					currentID = oscillatorIDerator;

					//add currentVertex here?
					grid->m_vertexMatrix[oscillatorIDerator].x = vertex[vertIt].x;
					grid->m_vertexMatrix[oscillatorIDerator].y = vertex[vertIt].y;
					grid->m_vertexMatrix[oscillatorIDerator].z = vertex[vertIt].z;
					grid->m_vertexMatrix[oscillatorIDerator].r = 0.0;
					grid->m_vertexMatrix[oscillatorIDerator].g = 1.0;
					grid->m_vertexMatrix[oscillatorIDerator].b = 1.0;
					grid->m_vertexMatrix[oscillatorIDerator].nx = normalV.x;
					grid->m_vertexMatrix[oscillatorIDerator].ny = normalV.y;
					grid->m_vertexMatrix[oscillatorIDerator].nz = normalV.z;
					oscillatorIDerator++;
				}
				idis[vertIt] = currentID;
			}
			tri->id_1 = idis[1];
			tri->id_3 = idis[2];
			tri->id_2 = idis[0];
			//add neighbors

			double distance;
			for (int vertIt = 0; vertIt < 3; vertIt++)
			{
				grid->m_mesh[tri->id_1]->addNeighbour(grid->m_mesh[tri->id_2]);
				grid->m_mesh[tri->id_2]->addNeighbour(grid->m_mesh[tri->id_1]);

				grid->m_mesh[tri->id_1]->addNeighbour(grid->m_mesh[tri->id_3]);
				grid->m_mesh[tri->id_3]->addNeighbour(grid->m_mesh[tri->id_1]);

				grid->m_mesh[tri->id_2]->addNeighbour(grid->m_mesh[tri->id_3]);
				grid->m_mesh[tri->id_3]->addNeighbour(grid->m_mesh[tri->id_2]);

				distance = sqrt(pow((grid->m_mesh[tri->id_1]->m_x - grid->m_mesh[tri->id_2]->m_x), 2) +
					pow((grid->m_mesh[tri->id_1]->m_y - grid->m_mesh[tri->id_2]->m_y), 2) +
					pow((grid->m_mesh[tri->id_1]->m_z - grid->m_mesh[tri->id_2]->m_z), 2));
				if (grid->_minNodalSpacing > distance)
				{
					grid->_minNodalSpacing = distance;
				}
				if (grid->_maxNodalSpacing < distance)
				{
					grid->_maxNodalSpacing = distance;
				}
			}
			//add triangle
			grid->m_vertexList.push_back(tri);
		}
	}

	grid->maxPotential = grid->maxElectrogram;
	grid->minPotential = grid->minElectrogram;
	//for (int currentVertex = 0; currentVertex < grid->m_mesh.size(); ++currentVertex)
	//{
	//}
	grid->m_indicesMatrix = (GLuint *)malloc(sizeof(GLuint) * 3 * grid->m_vertexList.size());
	for (int currentVertex = 0; currentVertex < grid->m_vertexList.size(); currentVertex = ++currentVertex)
	{
		grid->m_indicesMatrix[3 * currentVertex] = grid->m_vertexList[currentVertex]->id_1;
		grid->m_indicesMatrix[3 * currentVertex + 1] = grid->m_vertexList[currentVertex]->id_2;
		grid->m_indicesMatrix[3 * currentVertex + 2] = grid->m_vertexList[currentVertex]->id_3;
	}

	grid->calculateCenter();
	grid->setWallCells();
	grid->setDiffusionCoefficients();
	return grid;
}
CardiacMesh* CardiacMesh::constructCylindricalGrid(int x, int y, double dx, double dy)
{
	CardiacMesh *grid = new CardiacMesh();
	return grid;
}
CardiacMesh* CardiacMesh::importGrid(const char *inname)
{

	//[3] Read matlab variable VARNAME_MODEL_DATA
	mat_t *matfp;
	matvar_t *matvar;
	matfp = Mat_Open(inname, MAT_ACC_RDONLY);


	//[3] Read matlab variable oscillator_ID
	matvar = Mat_VarRead(matfp, "oscillator_ID");

	int meshSize = matvar->dims[0];
	INT32 *oscillator_ID = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	oscillator_ID = static_cast<INT32*>(matvar->data);

	//[3] Read matlab variable position_xyz
	matvar = Mat_VarRead(matfp, "position_xyz");

	double *position_xyz = (double *)malloc(sizeof(double) * 3 * meshSize);
	position_xyz = static_cast<double*>(matvar->data);


	//[3] Read matlab variable sigma
	matvar = Mat_VarRead(matfp, "sigma");

	double *sigma = (double *)malloc(sizeof(double) * 1 * meshSize);
	sigma = static_cast<double*>(matvar->data);

	matvar = Mat_VarRead(matfp, "erp_parameter");

	double *erp_parameter = (double *)malloc(sizeof(double) * 1 * meshSize);
	erp_parameter = static_cast<double*>(matvar->data);

	//[3] Read matlab variable oscillator_TYPE
	matvar = Mat_VarRead(matfp, "oscillator_TYPE");

	INT32 *oscillator_TYPE = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	oscillator_TYPE = static_cast<INT32*>(matvar->data);

	//[3] Read matlab variable oscillator_TYPE
	matvar = Mat_VarRead(matfp, "neighbours_ID");

	INT32 *neighbours_ID = (INT32 *)malloc(sizeof(INT32) * 6 * meshSize);
	neighbours_ID = static_cast<INT32*>(matvar->data);


	Mat_Close(matfp);

	CardiacMesh *grid = new CardiacMesh();
	int totalSize = meshSize;

	grid->maxElectrogram = -1999;
	grid->minElectrogram = 999;

	Oscillator *node = NULL;
	for (int j = 0; j < totalSize; ++j)
	{
		int temptype = oscillator_TYPE[j];
		CELL_TYPE type = static_cast<CELL_TYPE> (temptype);
		if (type == ATRIAL_V3)
			node = new v3model();
		else if (type == SOLID_WALL)
			node = new Oscillator();
		else
			node = new Oscillator();

		node->setPositionX(position_xyz[totalSize * 0 + j]);
		node->setPositionY(position_xyz[totalSize * 1 + j]);
		node->setPositionZ(position_xyz[totalSize * 2 + j]);
		node->setType(type);
		node->oscillatorID = oscillator_ID[j];
		grid->m_mesh.push_back(node);

		if (grid->maxElectrogram < node->vmax)
			grid->maxElectrogram = node->vmax;

		if (grid->minElectrogram > node->vmin)
			grid->minElectrogram = node->vmin;
	}

	grid->maxPotential = grid->maxElectrogram;
	grid->minPotential = grid->minElectrogram;
	//ADD NEIGHBOURS
	double deltaMin = DBL_MAX;
	double deltaMax = 0.0;
	double mindim = 0;
	double maxdim = 0;
	double mintemp = 0;
	double maxtemp = 0;
	for (int j = 0; j < totalSize; ++j)
	{
		int ng = 0;
		while (neighbours_ID[totalSize* ng + j] != -1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[neighbours_ID[totalSize* ng + j]]);
			++ng;
		}

		if (deltaMin > grid->m_mesh[j]->m_closestDistanceID)
			deltaMin = grid->m_mesh[j]->m_closestDistanceID;
		if (deltaMax < grid->m_mesh[j]->m_farthestDistanceID)
			deltaMax = grid->m_mesh[j]->m_farthestDistanceID;

		mintemp = min(grid->m_mesh[j]->m_x, grid->m_mesh[j]->m_y);
		mintemp = min(mintemp, grid->m_mesh[j]->m_z);
		if (mindim > mintemp)
		{
			mindim = mintemp;
		}
		maxtemp = max(grid->m_mesh[j]->m_x, grid->m_mesh[j]->m_y);
		maxtemp = max(maxtemp, grid->m_mesh[j]->m_z);
		if (maxdim < maxtemp)
		{
			maxdim = maxtemp;
		}
	}
	for (int j = 0; j < totalSize; ++j)
	{
		grid->m_mesh[j]->setSigma(sigma[j], sigma[j], 0);
		grid->m_mesh[j]->setERP(erp_parameter[j]);
		
	}

	grid->_minNodalSpacing = deltaMin;
	grid->_maxNodalSpacing = deltaMax;
	grid->_sizeInMm = maxdim;

	grid->setVertexTriangleList(false);
	grid->calculateCenter();
	grid->setWallCells();

	return grid;
}

void CardiacMesh::destroyGrid()
{
	while (!m_mesh.empty()) delete m_mesh.back(), m_mesh.pop_back();
}

//------------------------------------------------------------------------------------------
void CardiacMesh::addNode(int idnode, double x, double y, double z, CELL_TYPE type)
{
	Oscillator *node = NULL;
	if (type == ATRIAL_V3)
		node = new v3model();
	else if (type == ATRIAL_FHN)
		node = new FitzHughNagumo();

	else if (type == SA_NODE)
		node = new vanDerGru(SA_NODE);

	else if (type == AV_NODE)
		node = new vanDerGru(AV_NODE);
	else
		node = new Oscillator();

	node->setPositionX(x);
	node->setPositionY(y);
	node->setPositionZ(z);

	node->setType(type);
	node->oscillatorID = idnode;
	m_mesh.push_back(node);

	if (maxElectrogram < node->vmax)
		maxElectrogram = node->vmax;

	if (minElectrogram > node->vmin)
		minElectrogram = node->vmin;

}
void CardiacMesh::setVertexTriangleList(bool doublesided)
{
	int meshSize = m_mesh.size();
	if (m_vertexMatrix != nullptr)
		free(m_vertexMatrix);

	if (m_indicesMatrix != nullptr)
		free(m_indicesMatrix);

	m_vertexMatrix = (SVertex *)malloc(sizeof(SVertex) * 1 * meshSize);
	for (int currentVertex = 0; currentVertex < m_mesh.size(); ++currentVertex)
	{
		m_vertexMatrix[currentVertex].x = m_mesh[currentVertex]->m_x;
		m_vertexMatrix[currentVertex].y = m_mesh[currentVertex]->m_y;
		m_vertexMatrix[currentVertex].z = m_mesh[currentVertex]->m_z;
		m_vertexMatrix[currentVertex].r = 0.0;
		m_vertexMatrix[currentVertex].g = 1.0;
		m_vertexMatrix[currentVertex].b = 1.0;
	}


	m_vertexList.clear();
	Vector3 first;
	Vector3 second;
	for (int currentVertex = 0; currentVertex < m_mesh.size(); ++currentVertex)
	{
		int index = 0;
		bool ckeckNeighbours = true;
		for (int k = 0; k < m_vertexList.size(); ++k)
		{
			if (currentVertex == m_vertexList[k]->id_2 || currentVertex == m_vertexList[k]->id_3 || currentVertex == m_vertexList[k]->id_1)
			{
				ckeckNeighbours = false;
			}
		}
		if (ckeckNeighbours)
		{
			for (int currentNeighbour = 0; currentNeighbour < m_mesh[currentVertex]->m_neighbours.size(); ++currentNeighbour)
			{

				for (int i = currentNeighbour + 1; i < m_mesh[currentVertex]->m_neighbours.size(); ++i)
				{
					VertexTriangle *tr = new VertexTriangle;
					bool isok = true;
					tr->id_1 = currentVertex;
					tr->id_2 = m_mesh[currentVertex]->m_neighbours[currentNeighbour]->oscillatorID;
					tr->id_3 = m_mesh[currentVertex]->m_neighbours[i]->oscillatorID;

					first.x = m_mesh[tr->id_2]->m_x - m_mesh[tr->id_1]->m_x;
					first.y = m_mesh[tr->id_2]->m_y - m_mesh[tr->id_1]->m_y;
					first.z = m_mesh[tr->id_2]->m_z - m_mesh[tr->id_1]->m_z;
					second.x = m_mesh[tr->id_3]->m_x - m_mesh[tr->id_1]->m_x;
					second.y = m_mesh[tr->id_3]->m_y - m_mesh[tr->id_1]->m_y;
					second.z = m_mesh[tr->id_3]->m_z - m_mesh[tr->id_1]->m_z;

					if (first.normalize().dot(second.normalize()) < -0.99)
					{
						isok = false;
					}

					if (isok == true)
					{
						VertexTriangle *tr2 = new VertexTriangle;
						tr2->id_1 = currentVertex;
						tr2->id_2 = m_mesh[currentVertex]->m_neighbours[i]->oscillatorID;
						tr2->id_3 = m_mesh[currentVertex]->m_neighbours[currentNeighbour]->oscillatorID;
						if (doublesided)
						{
							m_vertexList.push_back(tr2);
							m_vertexList.push_back(tr);
						}
						else
						{
							if (first.cross(second).z >= 0 && m_mesh[currentVertex]->m_z >= 0) m_vertexList.push_back(tr2);
							else if (first.cross(second).z >= 0 && m_mesh[currentVertex]->m_z <= 0) m_vertexList.push_back(tr);
							else if (first.cross(second).z <= 0 && m_mesh[currentVertex]->m_z >= 0) m_vertexList.push_back(tr);
							else if (first.cross(second).z <= 0 && m_mesh[currentVertex]->m_z <= 0) m_vertexList.push_back(tr2);
							else if (first.cross(second).normalize().z > -0.05 && first.cross(second).normalize().z < 0.05)
							{
								m_vertexList.push_back(tr);
								m_vertexList.push_back(tr2);

							}

						}
					}
				}
			}
		}
	}
	m_indicesMatrix = (GLuint *)malloc(sizeof(GLuint) * 3 * m_vertexList.size());
	for (int currentVertex = 0; currentVertex < m_vertexList.size(); currentVertex = ++currentVertex)
	{
		m_indicesMatrix[3 * currentVertex] = m_vertexList[currentVertex]->id_1;
		m_indicesMatrix[3 * currentVertex + 1] = m_vertexList[currentVertex]->id_2;
		m_indicesMatrix[3 * currentVertex + 2] = m_vertexList[currentVertex]->id_3;
	}
}
void CardiacMesh::calculateCenter()
{
	centerGeom.x = 0;
	centerGeom.y = 0;
	centerGeom.z = 0;

	for (int currentVertex = 0; currentVertex < m_mesh.size(); ++currentVertex)
	{
		centerGeom.x += m_mesh[currentVertex]->getPositionX();
		centerGeom.y += m_mesh[currentVertex]->getPositionY();
		centerGeom.z += m_mesh[currentVertex]->getPositionZ();
	}
	centerGeom.x /= m_mesh.size();
	centerGeom.y /= m_mesh.size();
	centerGeom.z /= m_mesh.size();
}
const double CardiacMesh::sizeInMm()
{
	return _sizeInMm;

}
const double CardiacMesh::minNodalSpacing()
{
	return (this->_minNodalSpacing);
}

//------------------------------------------------------------------------------------------
void CardiacMesh::clearWallCells()
{
	for (int currentOsc = 0; currentOsc < m_mesh.size(); ++currentOsc)
	{
		m_mesh[currentOsc]->m_wallCells.clear();
	}
}
void CardiacMesh::setWallCells()
{
	clearWallCells();
	for (int currentOsc = 0; currentOsc < m_mesh.size(); ++currentOsc)
	{
		if (m_mesh[currentOsc]->m_type == SOLID_WALL)
		{
			Oscillator* neigh;
			for (int currentNeighbour = 0; currentNeighbour < m_mesh[currentOsc]->m_neighbours.size(); ++currentNeighbour)
			{
				neigh = m_mesh[currentOsc]->m_neighbours[currentNeighbour];
				if (neigh->getCellType() != SOLID_WALL &&
					neigh->getCellType() != NONE)
				{
					neigh->m_wallCells.push_back(m_mesh[currentOsc]);
					break;
				}
			}
		}
	}
}
void CardiacMesh::setDiffusionCoefficients()
{
	int gridSize = m_mesh.size();
	for (int j = 0; j < gridSize; ++j)
	{
		m_mesh[j]->setSigma(diffusion_CV20, diffusion_CV20, 0);
	}
}
//---------------------------------------------------------------------------
double CardiacMesh::calculateElectrogramRecursively(Oscillator* srcosc, Oscillator* osc, int currentLevel, std::set<int>& setOfadded)
{
	double ele_val = 0;

	for (unsigned int i = 0; i < osc->m_neighbours.size(); ++i)
	{
		//check if an oscillator were not added before
		if (setOfadded.find(osc->m_neighbours[i]->oscillatorID) == setOfadded.end())
		{
			setOfadded.insert(osc->m_neighbours[i]->oscillatorID);

			double distanceSQ = pow(srcosc->getPositionX() - osc->m_neighbours[i]->getPositionX(), 2) +
				pow(srcosc->getPositionY() - osc->m_neighbours[i]->getPositionY(), 2) +
				pow(srcosc->getPositionZ() - osc->m_neighbours[i]->getPositionZ(), 2);

			ele_val += osc->getLastCurrentSource() / (distanceSQ + 0.2);
			

			if (currentLevel > 0 && sqrt(distanceSQ) > this->minNodalSpacing()*ELECTR_DEPTH)
			{
				ele_val += calculateElectrogramRecursively(srcosc, osc->m_neighbours[i], currentLevel - 1, setOfadded);
			}
		}
	}

	return ele_val;
}
double CardiacMesh::calculateElectrogram(Oscillator* osc)
{
	int meshSize = m_mesh.size();
	double ele_val = 0;

	#ifdef CALCULATE_ELECTR_TOTAL
		Oscillator *osc2;
		for (int j2 = 0; j2 < meshSize; ++j2)//12; ++j)//
		{
			osc2 = m_mesh[j2];
			if (m_mesh[j2]->getCellType() != SOLID_WALL)
			{
				ele_val += osc2->getLastCurrentSource() / (pow(osc->getPositionX() - osc2->getPositionX(), 2) +
					pow(osc->getPositionY() - osc2->getPositionY(), 2) +
					pow(osc->getPositionZ() - osc2->getPositionZ(), 2) +
					0.7);
			}
		}
		osc->m_v_electrogram = ele_val;
	#else
		int search_levels = ELECTR_DEPTH;
		std::set<int> myset;

		ele_val = calculateElectrogramRecursively(osc, osc, search_levels, myset);
		osc->m_v_electrogram = ele_val;

	#endif
	return ele_val;
}
void CardiacMesh::calculateElectrogramGrid()
{
	double ele_val = 0;

	for (int j = 0; j < gridElectrode->_oscMatrix.size(); ++j)
	{
		Oscillator * osc = m_mesh[gridElectrode->_oscMatrix[j]];
		gridElectrode->m_historyPotential[j].push_back(osc->m_v_scaledPotential);// osc->m_v_scaledPotential);
		double electrogram = calculateElectrogram(osc);
		gridElectrode->m_historyElectrogram[j].push_back(electrogram);// osc->m_v_scaledPotential);
		gridElectrode->m_historyTime[j].push_back(osc->getCurrentTime());// osc->m_v_scaledPotential);
	}
}
//-----------------------------------------------------------
//-----------------------------------------------------------
void CardiacMesh::processActivationTime(Oscillator* osc)
{
	//[0] Read the Transmembrane potential value from the cell
	double lastLocalTimestep = osc->m_currentTime - osc->m_previousTime;
	double derivative = (osc->m_v_scaledPotential - osc->m_previous_scaledPotential) / lastLocalTimestep;

	//[1] Simple peak (depolarisation) detection algorithm for transmembrane potential;

	if (derivative > _derivativeTreshold &&
		(osc->getCurrentTime() - osc->m_lastActivationTime) > _minRefractoryTreshold)
	{
		osc->m_beforeLastActivationTime = osc->m_lastActivationTime;
		osc->m_lastActivationTime = osc->getCurrentTime();
	}
	//osc->m_potentialHistory = osc->m_v_scaledPotential;
}
//-----------------------------------------------------------
Vector3 CardiacMesh::conductionVector(int oscID)
{
	double distance = 1.0;
	m_markedOscillators.clear();
	m_marked.clear();
	calculateConductionVector(m_mesh[oscID], m_mesh[oscID], distance);

	std::vector<double> scores;
	for (short k = 0; k < m_markedOscillators.size(); ++k)
	{
		scores.push_back(m_markedOscillators[k]->m_lastActivationTime);
	}
	 double median;
	  size_t size = scores.size();

	  sort(scores.begin(), scores.end());

	  if (size  % 2 == 0)
	  {
		  median = (scores[size / 2 - 1] + scores[size / 2]) / 2;
	  }
	  else 
	  {
		  median = scores[size / 2];
	  }
	  median = median; //TODO 10 ms difference
	int counter = 0;
	while (counter < m_markedOscillators.size())
	{
		if (abs(m_markedOscillators[counter]->m_lastActivationTime- median) > 10) {
			m_markedOscillators.erase(m_markedOscillators.begin() + counter);
		}
		else {
			++counter;
		}
	}
	Vector3 *vecs = (Vector3 *)malloc(sizeof(Vector3) * m_markedOscillators.size());
	for (short k = 0; k < m_markedOscillators.size(); ++k)
	{
		vecs[k] = Vector3(m_markedOscillators[k]->m_x, m_markedOscillators[k]->m_y, m_markedOscillators[k]->m_lastActivationTime);
	}
	Vector3 output;
	Vector3 centerVec;

	FindLLSQPlane(vecs, m_markedOscillators.size(), &centerVec, &output);

	free(vecs);
	//output.z = 0;
		return output;
}
void CardiacMesh::calculateConductionVector(Oscillator* src, Oscillator* osc, const double radius)
{
	m_marked[osc->oscillatorID] = true;
	m_markedOscillators.push_back(osc);

	double distance;
	for (short k = 0; k < osc->m_neighbours.size(); ++k)
	{
		distance = sqrt(std::pow((src->m_x - osc->m_neighbours[k]->m_x), 2) +
			std::pow((src->m_y - osc->m_neighbours[k]->m_y), 2) +
			std::pow((src->m_z - osc->m_neighbours[k]->m_z), 2));
		if (distance <= radius && osc->m_neighbours[k]->getCellType() != SOLID_WALL &&!(m_marked[osc->m_neighbours[k]->oscillatorID]))
		{
			calculateConductionVector(src, osc->m_neighbours[k], radius);
		}
	}
}

float CardiacMesh::FindLargestEntry(const Matrix3 &m){
	float result = 0.0f;
	for (int i = 0; i<3; i++){
		for (int j = 0; j<3; j++){
			float entry = fabs(m[3 * i + j]);
			result = max(entry, result);
		}
	}
	return result;
}
Vector3 CardiacMesh::FindEigenVectorAssociatedWithLargestEigenValue(const Matrix3 &m){
	//pre-condition
	float scale = FindLargestEntry(m);
	Matrix3 mc = (1.0f / scale)*m;
	mc = mc*mc;
	mc = mc*mc;
	mc = mc*mc;
	Vector3 v(1, 1, 1);
	Vector3 lastV = v;
	for (int i = 0; i<100; i++){
		v = (mc*v).normalize();
		if ((v.distance(lastV)*v.distance(lastV))<1e-16f)
		{
			break;
		}
		lastV = v;
	}
	return v;
}
void CardiacMesh::FindLLSQPlane(Vector3 *points, int count, Vector3 *destCenter, Vector3 *destNormal){
	Vector3 sum(0, 0, 0);
	for (int i = 0; i<count; i++){
		sum += points[i];
	}
	Vector3 center = sum*(1.0f / count);
	if (destCenter){
		*destCenter = center;
	}
	if (!destNormal){
		return;
	}
	float sumXX = 0.0f, sumXY = 0.0f, sumXZ = 0.0f;
	float sumYY = 0.0f, sumYZ = 0.0f;
	float sumZZ = 0.0f;
	for (int i = 0; i<count; i++){
		float diffX = points[i].x - center.x;
		float diffY = points[i].y - center.y;
		float diffZ = points[i].z - center.z;
		sumXX += diffX*diffX;
		sumXY += diffX*diffY;
		sumXZ += diffX*diffZ;
		sumYY += diffY*diffY;
		sumYZ += diffY*diffZ;
		sumZZ += diffZ*diffZ;
	}
	Matrix3 m(sumXX, sumXY, sumXZ, \
		sumXY, sumYY, sumYZ, \
		sumXZ, sumYZ, sumZZ);

	float det = m.getDeterminant();
	if (det == 0.0f){
		//m.GetNullSpace(destNormal, NULL, NULL);
		return;
	}
	Matrix3 mInverse = m.invert();
	*destNormal = FindEigenVectorAssociatedWithLargestEigenValue(mInverse);
}