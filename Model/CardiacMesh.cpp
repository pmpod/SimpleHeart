#include "CardiacMesh.h"
#include <cmath>
#include <algorithm>
#include <set>
#include <matio.h>
#include <stdlib.h>
#include <baseTSD.h>

CardiacMesh::CardiacMesh()
{
	maximumCV = 1; // [cm/ms]
	maxDiffusion = 0.5;
	minDiffusion = 0.0;
	maxERP = 300;
	minERP = 10;
	maxElectrogram = -1999;
	minElectrogram = 999;
	stimulationBegun = false;


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
void CardiacMesh::destroyGrid()
{
	while (!m_mesh.empty()) delete m_mesh.back(), m_mesh.pop_back();
}


const double CardiacMesh::sizeInMm()
{
	return _sizeInMm;

}
const double CardiacMesh::minNodalSpacing()
{
	return (this->_minNodalSpacing);
}


CardiacMesh* CardiacMesh::constructCartesianGrid(int x, int y, double dx, double dy, CELL_TYPE type)
{
	CardiacMesh *grid = new CardiacMesh();
	int totalSize = y*x;

	grid->_minNodalSpacing = max(dx, dy);
	grid->_maxNodalSpacing = min(dx, dy);
	grid->_sizeInMm = sqrt(static_cast<double>(x*x + y*y))*dx;
	Oscillator *node = NULL;
	for (int j = 0; j < totalSize; ++j)
	{
		if (type == ATRIAL_V3)
			node = new v3model();
		else
			node = new Oscillator();
		
		node->setPositionX( static_cast<double>(j % x) * dx );
		node->setPositionY( dy * floor(static_cast<double>(j) / static_cast<double>(x)) );
		node->setPositionZ( 0.0 );
		node->setType(type);
		node->oscillatorID = j;
		grid->m_mesh.push_back(node);

		if (grid->maxElectrogram < node->vmax)
			grid->maxElectrogram = node->vmax;

		if (grid->minElectrogram > node->vmin)
			grid->minElectrogram = node->vmin;
	}
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
	}

	grid->_minNodalSpacing = deltaMin;
	grid->_maxNodalSpacing = deltaMax;
	grid->_sizeInMm = maxdim;

	grid->setVertexTriangleList(false);
	grid->calculateCenter();
	grid->setWallCells();

	return grid;
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
//------------------------------------------------------------------------------------------
void CardiacMesh::setDiffusionCoefficients()
{
	int gridSize = m_mesh.size();
	for (int j = 0; j < gridSize; ++j)
	{
		m_mesh[j]->setSigma(0.1, 0.1, 0);
	}
}
//---------------------------------------------------------------------------
double CardiacMesh::calculateElectrogram(Oscillator* osc)
{
	int meshSize = m_mesh.size();

	//ELECTROGRAM
	double ele_val = 0;
	Oscillator *osc2;
	for (int j2 = 0; j2 < meshSize; ++j2)//12; ++j)//
	{
		osc2 = m_mesh[j2];
		if (m_mesh[j2]->getCellType() != SOLID_WALL)
		{
			ele_val += osc2->getLastCurrentSource() / (pow(osc->getPositionX() - osc2->getPositionX(), 2) +
				pow(osc->getPositionY() - osc2->getPositionY(), 2) +
				0.7);
		}
	}
	osc->m_v_electrogram = ele_val;
	return ele_val;
	//END ELECTROGRAM
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
							else if (first.cross(second).normalize().z >-0.05 && first.cross(second).normalize().z <0.05)
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
		m_indicesMatrix[3 * currentVertex+1] = m_vertexList[currentVertex]->id_2;
		m_indicesMatrix[3 * currentVertex+2] = m_vertexList[currentVertex]->id_3;
	}
}
//-----------------------------------------------------------
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
