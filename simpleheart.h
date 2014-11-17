#ifndef SIMPLEHEART_H
#define SIMPLEHEART_H

#include <QtGui/QMainWindow>
#include <QObject>
#include "ui_simpleheart.h"
#include "View\glAtrium.h"
#include "AtrialMachine2d.h"
#include "DiffusionMatrix.h"
#include "DiffusionPainter.h"
#include "Model\CardiacMesh.h"
#include "timePlot.h"
#include "rrPlot.h"
#include "ioHandler.h"


class SimpleHeart : public QMainWindow
{
	Q_OBJECT

public:
	SimpleHeart(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SimpleHeart();

	AtrialMachine2d* Machine2d;
	glAtrium			*glGraph;
	DiffusionPainter* diffusionPainter;
	ioHandler	*m_ioHandler;


	CardiacMesh* m_grid;

	atrialParameters* simpleParameters;
	void init();
	void destroy();
	void setupConnections();

	
	std::vector<TimePlot*> plotPotentials;
	
	std::vector<rrPlot*>	plotRR;
	
	rrPlot				*plotEnt_el1;
	rrPlot				*plotEnt_el2;
	rrPlot				*plotEnt_el3;

	QMessageBox* msgBoxAbout;
	Ui::SimpleHeartClass ui;

protected:
    virtual void timerEvent(QTimerEvent *e);

signals:

	void tajmer();
	void emitGlobalTimeOnly(double);

public slots:
   
	 
	void reset();
    void setTimerInterval(double interval);
	void stopCalculation();
	void startCalculation();

	void openInstruction();
	void openSettings();
	void resetDiffusion();
	void setAtrialDiffusion();
	void setAtrialStructure();
	void setAtrialRestitution(int value);
	void setEctopicPoint();
	void setEctopicHoriz();
	void setEctopicVert();
	void setEctopicPeriodS1(double v);
	void setEctopicModS1(bool b);
	void setEctopicPeriodS2(double v);
	void setEctopicModS2(bool b);
	
	void setEntropyToggle(bool v);
	void setEntropyPat(bool v);
	void setEntropyVar(bool v);

	void plotPotentialClear();
	void plotRRClear();
	void plotEntClear();


	void setProbeElectrode(int electrode, int id_y, int id_x);
	void switchElectrode();

	void setBmpSaving(bool t);
	


private:

	bool m_calculationsAreRunning;
	bool m_calculationsArePaused;
	
	bool m_patternEnt;
	bool m_variabilityEnt;
	
	DiffusionMatrix* m_matrix;
	DiffusionMatrix* m_anisotrophy;
	QVBoxLayout *layout_Visualisation;
	QVBoxLayout *layout_Diffusion;
	QVBoxLayout *layout_Plots;
	QHBoxLayout *layout_PlotsRR;
	QHBoxLayout *layout_PlotsEnt;

	int		d_interval;	
    int		d_timerId;

	double refreshRate;
	double lastRefreshTime;
	double refreshCounter;
};

#endif // SIMPLEHEART_H
