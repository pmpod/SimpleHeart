#include "cartesianGrid.h"
//-----------------------------------------------------------
CartesianGrid::CartesianGrid(int x, int y, double dx, double dy)
{
	init(x, y, dx, dy);
	
}
//-----------------------------------------------------------
CartesianGrid::~CartesianGrid(void)
{

}
//-----------------------------------------------------------
void CartesianGrid::constructGrid(int x, int y, double dx, double dy)
{
	destroyGrid();

	m_dx = dx;
	m_dy = dy;

	for (int j = 0; j < y; ++j)
	{
		std::vector<Node*> temp_line;
		m_net.push_back( temp_line );
		for (int i = 0; i < x; ++i)
		{
			Node * newNode;
			newNode = new Node;
			newNode->m_x = i;
			newNode->m_y = j;
			newNode->m_anisotrophy_L=0.5;
			newNode->m_anisotrophy_R=0.5;
			newNode->m_type = ATRIAL_V3;
			newNode->m_color = QColor(255,255,255);
			newNode->m_group = 0;
			m_net[j].push_back( newNode );
			newNode->seleced1 = false;
			newNode->seleced2 = false;
		}
	}

}
//-----------------------------------------------------------
void CartesianGrid::destroyGrid()
{
	for( vector<vector<Node*>>::iterator it = m_net.begin(); it != m_net.end(); ++it)
		for( vector<Node*>::iterator it2 = it->begin(); it2 != it->end(); ++it2)
			delete *it2;  
}
//-----------------------------------------------------------
void CartesianGrid::init(int x, int y, double dx, double dy)
{
	constructGrid(x, y, dx, dy);

	//for (int j = 0; j < (y/8); ++j)
	//{
	//	for (int i = 0; i < (x/8); ++i)
	//	{
	//		m_net[j][i]->m_type = SA_NODE;
	//		m_net[j][i]->m_color = QColor(255,255,0);
	//		m_net[j][i]->m_group = 0;
	//	}
	//}
	//for (int j = y-(y/8); j < getSizeY(); ++j)
	//{
	//	for (int i = x - (x/8); i < getSizeX(); ++i)
	//	{
	//		m_net[j][i]->m_type = AV_NODE;
	//		m_net[j][i]->m_color = QColor(0,255,0);
	//		m_net[j][i]->m_group = 0;
	//	}
	//}
}
//--------------------------
void CartesianGrid::defaults()
{
	for (int j = 0; j < getSizeY(); ++j)
	{
		for (int i = 0; i < getSizeX(); ++i)
		{
			m_net[j][i]->m_type = ATRIAL_TISSUE;
			m_net[j][i]->m_color = QColor(255,255,255);
			m_net[j][i]->m_group = 0;
		}
	}
	for (int j = 0; j < (getSizeY()/8); ++j)
	{
		for (int i = 0; i < (getSizeX()/8); ++i)
		{
			m_net[j][i]->m_type = SA_NODE;
			m_net[j][i]->m_color = QColor(255,255,0);
			m_net[j][i]->m_group = 0;
		}
	}
	for (int j = getSizeY()-(getSizeY()/8); j < getSizeY(); ++j)
	{
		for (int i = getSizeX() - (getSizeX()/8); i < getSizeX(); ++i)
		{
			m_net[j][i]->m_type = AV_NODE;
			m_net[j][i]->m_color = QColor(0,255,0);
			m_net[j][i]->m_group = 0;
		}
	}
}
//-----------------------------------------------------------------
int CartesianGrid::getSizeX(){ return m_net[0].size(); }
int CartesianGrid::getSizeY(){ return m_net.size(); }
double CartesianGrid::getDeltaX(){ return m_dx; }
double CartesianGrid::getDeltaY(){ return m_dy; }