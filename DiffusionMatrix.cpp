#include "DiffusionMatrix.h"


DiffusionMatrix::DiffusionMatrix(CardiacMesh* grid)
{
	init(grid);
}
DiffusionMatrix::~DiffusionMatrix()
{
	clear();
}
void DiffusionMatrix::clear()
{
	for (int j = 0; j < m_grid->getSize(); ++j)
	{
		for (int i = 0; i < m_grid->getSize(); ++i)
		{
			m_matrix[j][i]->color = QColor(255,255,255);
		}

	}
	for( vector<vector<Pix*>>::iterator it = m_matrix.begin(); it != m_matrix.end(); ++it)
		for( vector<Pix*>::iterator it2 = it->begin(); it2 != it->end(); ++it2)
			delete *it2;
}
void DiffusionMatrix::resetDefault(int r, int g, int b)
{
	for (int j = 0; j < m_grid->getSize(); ++j)
	{
		for (int i = 0; i < m_grid->getSize(); ++i)
		{
			m_matrix[j][i]->color = QColor(r,g,b);
		}

	}
}
void DiffusionMatrix::init(CardiacMesh* grid)
{
	m_grid = grid;

	for (int j = 0; j < m_grid->getSize(); ++j)
	{
		vector<Pix*> temp;
		m_matrix.push_back(temp);
		for (int i = 0; i < m_grid->getSize(); ++i)
		{
			Pix *pikselek = new Pix();
			pikselek->size = 1;//min( this->width() /  m_grid->getSizeX(), this->height() / m_grid->getSizeY());
			pikselek->color = QColor(255,255,255);
			pikselek->id_x = i;
			pikselek->id_y = j;
			pikselek->position = QPoint( pikselek->size * i, pikselek->size * j);  
			m_matrix[j].push_back(pikselek);
		}

	}
}

void DiffusionMatrix::refresh()
{
	//for (int j = 0; j < m_grid->getSizeY(); ++j)
	//{
	//	for (int i = 0; i < m_grid->getSizeX(); ++i)
	//	{
	//		m_matrix[j][i]->size = 1;//min( this->width() /  m_grid->getSizeX(), this->height() / m_grid->getSizeY());
	//		m_matrix[j][i]->color = 
	//		m_matrix[j][i]->id_x = i;
	//		m_matrix[j][i]->id_y = j;
	//		m_matrix[j][i]->position = QPoint( m_matrix[j][i]->size * i, m_matrix[j][i]->size * j);
	//	}

	//}
}