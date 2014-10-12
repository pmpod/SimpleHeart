#pragma once
#include <QObject>
#include <QtGui>
#include <vector>
#include "Oscillator.h"
#include "vanDerGru.h"
#include "heartDefines.h"
class Node
{
public:
	int m_group;
	double m_x;
	double m_y;

	QColor m_color;
	CELL_TYPE m_type;
	bool seleced1;
	bool seleced2;
	double m_anisotrophy_R;
	double m_anisotrophy_L;
};
//--------------------------------------------
class CartesianGrid: public QObject
{
	Q_OBJECT

public:
	CartesianGrid(int x, int y, double dx, double dy);
	~CartesianGrid(void);

	void constructGrid(int x, int y, double dx, double dy);
	void destroyGrid();
	void init(int x, int y, double dx, double dy);
	void defaults();
	std::vector<std::vector<Node*>> m_net;
	int getSizeX();
	int getSizeY();
	double getDeltaX();
	double getDeltaY();
public slots:
	//changeNode(int x, int y, CELL_TYPE m_type);
private:

	double m_dx;
	double m_dy;

};
