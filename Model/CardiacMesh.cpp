#include "CardiacMesh.h"
#include <cmath>
#include <algorithm>
#include <set>

CardiacMesh::CardiacMesh()
{
	m_maximumCV = 1; // [cm/ms]
	stimulationBegun = false;
}


CardiacMesh::~CardiacMesh()
{
}

int CardiacMesh::changeNode(int x, int y, CELL_TYPE m_type)
{
	return 1;
}

double CardiacMesh::getSizeInMm()
{
	return (static_cast<double>(this->m_size) * (this->m_maximumDistance));
}
int CardiacMesh::getSize()
{
	return this->m_size;
}
double CardiacMesh::getDeltaR()
{
	return (this->m_minimumDistance);
}


CardiacMesh* CardiacMesh::constructCartesianGrid(int x, int y, double dx, double dy, CELL_TYPE type)
{
	CardiacMesh *grid = new CardiacMesh();
	int totalSize = y*x;

	grid->m_minimumDistance = max(dx, dy);
	grid->m_maximumDistance = min(dx, dy);
	grid->m_size = min(x, y);

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
	}

	//ADD NEIGHBOURS
	int idx, idy;
	//x = x - 1;
	//y = y - 1;
	for (int j = 0; j < y*x; ++j)
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
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == x-1 && idy > 0 && idy < y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx > 0 && idx < x-1  && idy == 0 )
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx > 0 && idx < x-1  && idy == y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == 0 && idy == 0)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == x-1 && idy == y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == x-1 && idy == 0)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + x]); //dodaje s¹siedni oscylator
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}
		else if (idx == 0 && idy == y-1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j + 1]); //dodaje s¹siedni oscylator
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[j - x]); //dodaje s¹siedni oscylator
			//grid->m_mesh[j]->m_type = SOLID_WALL;
		}

	}

	grid->setVertexTriangleList(false);
	grid->calculateCenter();

	return grid;
}
CardiacMesh* CardiacMesh::constructCylindricalGrid(int x, int y, double dx, double dy)
{
	CardiacMesh *grid = new CardiacMesh();
	return grid;
}
CardiacMesh* CardiacMesh::importGrid()
{
	CardiacMesh *grid = new CardiacMesh();
	return grid;
}

void CardiacMesh::destroyGrid()
{
	while (!m_mesh.empty()) delete m_mesh.back(), m_mesh.pop_back();
}

void CardiacMesh::setStimulation(Oscillator* osc, const int& depth)
{	// mo¿na opszeæ o sprawdzanie odleg³oœci 
	if (!osc->m_underStimulation)
	{
		m_underStimulation.push_back(osc);
		osc->m_underStimulation = true;
	}
	for (int k = 0; k < osc->m_neighbours.size(); ++k)
	{ 
		if (depth > 0)
			setStimulation(osc->m_neighbours[k], depth - 1);
	}
	stimulationBegun = true;
}
void CardiacMesh::stopStimulation()
{
	for (int j = 0; j < m_underStimulation.size(); ++j)
	{
		m_underStimulation[j]->m_underStimulation = false;
	}
	m_underStimulation.clear();
	stimulationBegun = false;
}
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
			ele_val += osc2->getUniformTimestepCurrentSource() / (pow(osc->getPositionX() - osc2->getPositionX(), 2) +
				pow(osc->getPositionY() - osc2->getPositionY(), 2) +
				0.7);
		}
	}
	osc->setElectrogram(ele_val);
	return ele_val;
	//END ELECTROGRAM
}

//-----------------------------------------------------------
void CardiacMesh::setVertexTriangleList(bool doublesided)
{
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
							if (first.cross(second).z >= 0 && m_mesh[currentVertex]->m_z <= 0) m_vertexList.push_back(tr);
							if (first.cross(second).z <= 0 && m_mesh[currentVertex]->m_z >= 0) m_vertexList.push_back(tr);
							if (first.cross(second).z <= 0 && m_mesh[currentVertex]->m_z <= 0) m_vertexList.push_back(tr2);
							if (first.cross(second).normalize().z >-0.05 && first.cross(second).normalize().z <0.05)
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
