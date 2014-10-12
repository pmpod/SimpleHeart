#pragma once
#include <QObject>
#include <vector>
#include "Oscillator.h"
#include "v3model.h"
#include "heartDefines.h"
#include "TimeTree.h"
#include <set>
class CardiacMesh : public QObject
{
	Q_OBJECT


public:
	CardiacMesh();
	~CardiacMesh(void);

	static CardiacMesh *constructCartesianGrid(int x, int y, double dx, double dy, CELL_TYPE type);
	static CardiacMesh *constructCylindricalGrid(int x, int y, double dx, double dy);
	static CardiacMesh *importGrid();

	void destroyGrid();
	int getSize();
	double getSizeInMm();
	double getDeltaR();


	void setStimulation(Oscillator* osc, const int& depth);
	void stopStimulation();
	double calculateElectrogram(Oscillator* osc);
	bool stimulationBegun;
	double m_ectopicAmplitude;

	std::vector<Oscillator*> m_mesh;
	std::vector<Oscillator*> m_underStimulation;

	double m_minimumDistance;
	double m_maximumDistance;
	double m_maximumCV;

	int m_size;

	public slots:
	int	changeNode(int x, int y, CELL_TYPE m_type);

};
