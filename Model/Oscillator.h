#pragma once
#include <QObject>
#include <vector>
#include <heartDefines.h>

class TimeTree;

/*! \class Oscillator
 *  \brief Abstract class for cell implementation. Contains electrophysiology model (CEM) & local geometrical properties
 * Oscillator class is an abstract cell class, in which an electrophysiological model for the cell is implemented. 
 * The knowledge about the type of cell electrophysiology model is not  relevant to other parts of the program and is encapsulated 
 * in the concrete impementation of the model. Any implementation of CEM should inherit from this calss
 *
 */
class Oscillator: public QObject
{
	Q_OBJECT


public:
	CELL_TYPE m_type;	///Type of the cell (wall, non-wall, none, tissue, nodal, etc.)					
	int oscillatorID;	//ID number of the oscillator (matches location in the CardiacMesh m_osc vector )

	double m_v_potential;		/// Last cardiac potential V(t) - in model units [model]
	double m_previous_potential;	/// Previous cardiac potential V(t-1) - in model units [model]
	std::vector<double> m_v_current;/// Vector of last cardiac currents/gating variables/model variables - in model units [model]

	double m_potentialPRIM;		/// Last calculated value of dV/dt (TODO check if necessary/useful??)
	std::vector<double>  m_currentPRIM;/// Last calculated values of dC/dt (TODO check if necessary/useful??)

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
	std::vector<Oscillator*> m_wallCells;		///Vector of pointers to neighbour oscillators
	std::vector<Oscillator*> m_neighbours;		///Vector of pointers to neighbour oscillators
	std::vector<double> m_neighboursDistance;	///Vector of pointers to distance to neighbour oscillators [mm]
	std::vector<double> m_connexin;			///Vector of diffusion coefficients associated with neighbours

	double m_hormonalActivity;

	//TODO Skalowanie modelu external scaling
	double vmax;					///Model scaling values - V maximum [mV]
	double vmin;					///Model scaling values - V minimum [mV]
	double vzero;					///Model scaling values - V resting [mV]
	
public:
	Oscillator(void);
	~Oscillator(void);

	void stateCalculated(double a, double b, int which);	/// Qt emitter of full new state signal set
signals:
	void newPotentialValue(double potValue);		/// Qt signal of new (scaled) potential
	void newState(double potValue, double curValue);	/// Qt signal of new (scaled) potential & total current
	void newPotentialTime(double time, double potValue);	/// Qt signal of new (scaled) potential & time [ms]
public slots:
	virtual void setParameter(double value, OSC_PARAMETER parameter); ///virtual function for parameter setting

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
	
	CELL_TYPE getCellType();	///Returns the current cell type
	double getElectrogram();	///Returns the current electrogram value

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
	void reset();
};
