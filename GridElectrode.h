#pragma once
#include<vector>
#include <QObject>
#include "Model\Oscillator.h"
class GridElectrode : public QObject
{
	Q_OBJECT

public:
	GridElectrode();
	~GridElectrode();



		std::vector<Oscillator*> _oscMatrix;

		std::vector<std::vector<double>> m_historyElectrogram;
		std::vector<std::vector<int>> m_historyActivationTimes;
		std::vector<std::vector<double>> m_historyAfterHilbert;
		int _spacingXY; // distance between electrodes in oscillators
		int posX;
		int posY;
};

