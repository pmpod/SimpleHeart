#pragma once
#include<vector>
#include <QObject>
#include <ctime>
#include <sstream>
#include "Model\Oscillator.h"
#include "Support\Pawel_Numerical_Library.h"
class GridElectrode : public QObject
{
	Q_OBJECT

public:
	GridElectrode();
	GridElectrode(int spacing, int x, int y, int nx, int ny);
	void calculateHilbert();
	void reset();
	~GridElectrode();

	void setReferenceElectrodeXY();
	bool saveData();

		std::vector<int> _oscMatrix;

		std::vector<std::vector<double>> m_historyElectrogram;
		std::vector<std::vector<double>> m_historyPotential;
		std::vector<std::vector<double>> m_historyActivationTimes;
		std::vector<std::vector<double>> m_historyAfterHilbert;
		std::vector<std::vector<double>> m_historyTime;
		int _spacingXY; // distance between electrodes in oscillators
		int _numberOfElectrodesX; // distance between electrodes in oscillators
		int _numberOfElectrodesY; // distance between electrodes in 
		int posX; //lefttop electrode position
		int posY;//lefttop electrode position

		Pawel_Numerical_Library library;
};

