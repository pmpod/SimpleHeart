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