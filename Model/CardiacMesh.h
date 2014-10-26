#pragma once
#include <QObject>
#include <QMessageBox>
#include <vector>
#include "Model\Oscillator.h"
#include "Model\v3model.h"
#include "heartDefines.h"
#include "NumericStrategy\TimeTree.h"
#include "Support\Vectors.h"
#include <matio.h>

struct VertexTriangle
{
	int id_1;
	int id_2;
	int id_3;
};

class CardiacMesh : public QObject
{
	Q_OBJECT


public:
	CardiacMesh();
	~CardiacMesh(void);




	static CardiacMesh* constructCartesianGrid(int x, int y, double dx, double dy, CELL_TYPE type);
	static CardiacMesh* constructCylindricalGrid(int x, int y, double dx, double dy);
	static CardiacMesh* importGrid();
	void setWallCells();
	void clearWallCells();
	void setVertexTriangleList(bool doublesided);

	void destroyGrid();
	int getSize();
	double getSizeInMm();
	double getDeltaR();

	void startStimulation(Oscillator* osc, const int& id, const double radius, const double strength);
	//void setStimulation(Oscillator* osc, const int& depth);
	void stopStimulation();
	double calculateElectrogram(Oscillator* osc);
	bool stimulationBegun;
	double m_ectopicAmplitude;

	std::vector<Oscillator*> m_mesh;
	std::vector<Oscillator*> m_underStimulation;
	//std::vector<std::pair<Oscillator*, Oscillator*>> m_wallCells;

	double m_simulationTime;

	double m_minimumDistance;
	double m_maximumDistance;
	double m_maximumCV;

	double m_maxElectrogram;
	double m_minElectrogram;
	double m_maxPotential;
	double m_minPotential;



	//TODO - poprawiæ getSize na double !!!
	int m_size;


	//3d

	void calculateCenter();
	double m_radius;
	std::vector<VertexTriangle*> m_vertexList;
	Vector3 centerGeom;

	public slots:
	int	changeNode(int x, int y, CELL_TYPE m_type);


};
