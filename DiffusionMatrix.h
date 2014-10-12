#pragma once
#include <QColor>
#include <QPoint>
#include <vector>
#include "heartDefines.h"
#include "cartesianGrid.h"
#include "CardiacMesh.h"
class Pix
{
public:
	double size;
	QColor color;
	int id_x;
	int id_y;
	QPoint position;
};

class DiffusionMatrix
{
public:
	DiffusionMatrix(CardiacMesh* grid);
	~DiffusionMatrix();
	void init(CardiacMesh* grid);
	void refresh();
	void clear();
	void resetDefault(int r, int g, int b);
	vector<vector<Pix*>> m_matrix;

	CardiacMesh *m_grid;
};