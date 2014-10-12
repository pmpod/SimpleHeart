#pragma once
#include <QObject>
#include <vector>
#include <heartDefines.h>

class TimeTree;

class Oscillator: public QObject
{
	Q_OBJECT


public:
	CELL_TYPE m_type;						
	int oscillatorID;

	double m_v_potential;			//V(t)
	double m_previous_potential;	//V(t - m_lastTimestep)
	std::vector<double> m_v_current;

	double m_potentialPRIM;
	std::vector<double>  m_currentPRIM;

	double m_v_electrogram;
	double m_v_scaledPotential;
	double m_previous_scaledPotential;

	double m_currentTime;			//t(current)
	double m_previousTime;			//t(current-1)
	double guardTimestep;

	bool m_underStimulation;


	double _sourceA;
	double _sourceB;
	double m_currentSource;
	double m_x;
	double m_y;
	double m_z;
	double m_sigmaX;
	double m_sigmaY;
	double m_sigmaZ;
	double m_ConnexinSum;
	std::vector<Oscillator*> m_neighbours;//s¹siedzi danego oscylatora w sieci
	std::vector<double> m_neighboursDistance;//s¹siedzi danego oscylatora w sieci
	std::vector<double> m_connexin;//s¹siedzi danego oscylatora w sieci

	double m_hormonalActivity;

	//Skalowanie modelu
	double vmax;
	double vmin;
	double vzero;
	
public:
	Oscillator(void);
	~Oscillator(void);

	void stateCalculated(double a, double b, int which);				//emiter g³ównych sygna³ów
signals:
	void newPotentialValue(double potValue);		//sygna³ - z wartoœci¹ potencja³u
	void newState(double potValue, double curValue);//sygna³ z faz¹ systemu
	void newPotentialTime(double time, double potValue);
public slots:
    virtual void setParameter(double value, OSC_PARAMETER parameter);	//set oscillator parameter *virtual!!)

public:

	double& getRefPotential();				//zwraca m_v_potential
	double& getRefPreviousPotential();				//zwraca m_v_potential
	double& getRefCurrentTime();

	double getCurrent(const int& which);				//zwraca m_v_current
	double getPotential();				//zwraca m_v_potential
	double getPreviousPotential();				//zwraca m_v_potential

	virtual double getCurrentPrim(const int& which);	//podaj wynik wzoru na pochodn¹ pr¹du
	virtual double getPotentialPrim();  //podaj wynik wzoru na pochodn¹ potencja³u

	double getCurrentTime();
	double getPreviousTime();
	
	CELL_TYPE getCellType();
	double getElectrogram();  //podaj wynik wzoru na pochodn¹ potencja³u

	double getExtrapolatedNeighbourSource();					
	double getUniformTimestepCurrentSource();
	double getLastCurrentSource();		

	double getPositionX();
	double getPositionY();
	double getPositionZ();
	int getNumberOfCurrents();

	void setPositionX(const double& pos);
	void setPositionY(const double& pos);
	void setPositionZ(const double& pos);

	void setType(CELL_TYPE type);
	void addNeighbour(Oscillator* neighbour); //dodaje s¹siedni oscylator

	void setCurrent(const double& current, const int& which);	//ustawia m_v_Current
	void setElectrogram(const double& electrogram);
	void setPotential(const double& potential); //ustawia //m_v_potential
	void setPreviousPotential(const double& potential); //ustawia //m_v_potential

	void setCurrentTime(const double& time);

	void setSigma(double sX, double sY, double sZ);
	void setConnexins();
};
