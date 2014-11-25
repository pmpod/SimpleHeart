#include "simpleheart.h"
#include <sstream>



template <class T>
inline std::string to_string (const T& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}

SimpleHeart::SimpleHeart(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);


	init();
	
	m_ioHandler = new ioHandler(this);

	setupConnections();


	m_calculationsAreRunning = false;
	m_calculationsArePaused = false;
	//m_patternEnt = true;
	//m_variabilityEnt = false;
	//setEntropyPat(m_patternEnt);

	refreshRate = 1.0;
	lastRefreshTime = 0.0;
	ui.statusBar->showMessage(tr("Ready"));
}

SimpleHeart::~SimpleHeart()
{
	m_ioHandler->writeParametersToFile();
}

void SimpleHeart::reset()
{
	ui.statusBar->showMessage(tr("Simulation reset"));

	for (short k = 0; k < plotRR.size(); ++k)
	{
		QObject::disconnect(Machine2d->stimulator->probeElectrode(k), SIGNAL(nextRR(double)), plotRR[k], SLOT(appendRR(double)));
		plotRR[k]->clear();
		QObject::connect(Machine2d->stimulator->probeElectrode(k), SIGNAL(nextRR(double)), plotRR[k], SLOT(appendRR(double)));
	}
	plotRR[0]->d_curve.back()->setPen(QPen(QBrush(QColor(255, 255, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR[1]->d_curve.back()->setPen(QPen(QBrush(QColor(255, 0, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR[2]->d_curve.back()->setPen(QPen(QBrush(QColor(0, 255, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));

}
void SimpleHeart::init()
{
	layout_Visualisation = new QVBoxLayout;
	layout_Visualisation->setContentsMargins(1, 1,1, 1);
	layout_Visualisation->setSpacing(1);

	layout_Diffusion= new QVBoxLayout;
	layout_Diffusion->setContentsMargins(1, 1,1, 1);
	layout_Diffusion->setSpacing(1);

	layout_Plots= new QVBoxLayout;
	layout_Plots->setContentsMargins(1, 1,1, 1);
	layout_Plots->setSpacing(1);

	layout_PlotsRR= new QHBoxLayout;
	layout_PlotsRR->setContentsMargins(1, 1,1, 1);
	layout_PlotsRR->setSpacing(1);

	layout_PlotsEnt= new QHBoxLayout;
	layout_PlotsEnt->setContentsMargins(1, 1,1, 1);
	layout_PlotsEnt->setSpacing(1);


	//--------------------
	plotPotentials.push_back(new TimePlot(ui.displayGraphs));
	plotPotentials.push_back(new TimePlot(ui.displayGraphs));
	plotPotentials.push_back(new TimePlot(ui.displayGraphs));
	for (short k = 0; k < plotPotentials.size(); ++k)
	{
		plotPotentials[k]->setMinimumHeight(2);
		plotPotentials[k]->enableAxis(QwtPlot::xBottom, false);
		plotPotentials[k]->setAxisAutoScale(QwtPlot::yLeft);
	}
	plotPotentials[0]->setObjectName(QString::fromUtf8("Electrogram 1"));
	plotPotentials[0]->setAxisTitle(QwtPlot::yLeft, "Probe 1");
	plotPotentials[1]->setObjectName(QString::fromUtf8("Electrogram 2"));
	plotPotentials[1]->setAxisTitle(QwtPlot::yLeft, "Probe 2");
	plotPotentials[2]->setObjectName(QString::fromUtf8("Electrogram 3"));
	plotPotentials[2]->setAxisTitle(QwtPlot::yLeft, "Probe 3");
	plotPotentials[2]->enableAxis(QwtPlot::xBottom, true);
	//------------------------------
	plotRR.push_back(new rrPlot(ui.displayGraphs_3));
	plotRR.push_back(new rrPlot(ui.displayGraphs_3));
	plotRR.push_back(new rrPlot(ui.displayGraphs_3));
	for (short k = 0; k < plotRR.size(); ++k)
	{
		plotRR[k]->setObjectName(QString::fromUtf8("plot RR"));
		plotRR[k]->setAxisAutoScale(QwtPlot::yLeft);
		plotRR[k]->setAxisScale(QwtPlot::xBottom, 0, 99);
		plotRR[k]->setMinimumHeight(2);
	}
	plotRR[0]->d_curve.front()->setPen(QPen(QBrush(QColor(255, 255, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR[1]->d_curve.front()->setPen(QPen(QBrush(QColor(255, 0, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR[2]->d_curve.front()->setPen(QPen(QBrush(QColor(0, 255, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	//------------------------------
	//plotEnt_el1 = new rrPlot(ui.displayGraphs_3);
	//plotEnt_el1->setObjectName(QString::fromUtf8("plot Entropy"));
	//plotEnt_el1->setAxisAutoScale(QwtPlot::yLeft);
	//plotEnt_el1->setAxisScale(QwtPlot::xBottom, 0, 99);
	//plotEnt_el1->setMinimumHeight(2);

	//plotEnt_el1->d_curve.back()->setPen(QPen(Qt::yellow));
	//plotEnt_el2 = new rrPlot(ui.displayGraphs_3);
	//plotEnt_el2->setObjectName(QString::fromUtf8("plot Entropy"));
	//plotEnt_el2->setAxisAutoScale(QwtPlot::yLeft);
	//plotEnt_el2->setAxisScale(QwtPlot::xBottom, 0, 99);
	//plotEnt_el2->setMinimumHeight(2);
	//plotEnt_el2->d_curve.back()->setPen(QPen(Qt::red));
	//plotEnt_el3 = new rrPlot(ui.displayGraphs_3);
	//plotEnt_el3->setObjectName(QString::fromUtf8("plot Entropy"));
	//plotEnt_el3->setAxisAutoScale(QwtPlot::yLeft);
	//plotEnt_el3->setAxisScale(QwtPlot::xBottom, 0, 99);
	//plotEnt_el3->setMinimumHeight(2);
	//plotEnt_el3->d_curve.back()->setPen(QPen(Qt::green));













	simpleParameters = new atrialParameters();
	//m_grid = CardiacMesh::constructCartesianGrid(256, 256, 0.3125, 0.3125, ATRIAL_V3);
	m_grid = CardiacMesh::constructCartesianGrid(128, 128, 0.3125, 0.3125, ATRIAL_V3);// 4 
	//m_grid = CardiacMesh::constructCartesianGrid(128, 128, 0.375, 0.375, ATRIAL_V3);// 48 mm
	//m_grid = CardiacMesh::constructCartesianGrid(200, 200, 0.4, 0.4, ATRIAL_V3);// 80 mm
	//m_grid = CardiacMesh::constructCartesianGrid(256, 256, 0.3125, 0.3125, ATRIAL_V3);
	//m_grid = new CartesianGrid(256,256,0.05,0.05);
	//m_matrix = new DiffusionMatrix(m_grid);
	//m_anisotrophy = new DiffusionMatrix(m_grid);

	Machine2d = new AtrialMachine2d(simpleParameters, m_grid);
	glGraph = new glAtrium(m_grid,Machine2d, ui.displayMain);

	//diffusionPainter = new DiffusionPainter(m_grid, m_matrix,m_anisotrophy, ui.displayDiffusion);
	

	
//vis tab-----------------------------
	layout_Visualisation->addWidget( glGraph );
	ui.displayMain->setLayout(layout_Visualisation);
	//layout_Diffusion->addWidget( diffusionPainter );
	//ui.displayDiffusion->setLayout(layout_Diffusion);
	layout_Plots->addWidget( plotPotentials[0] );
	layout_Plots->addWidget( plotPotentials[1] );
	layout_Plots->addWidget(plotPotentials[2]);
	ui.displayGraphs->setLayout(layout_Plots);
	layout_PlotsRR->addWidget( plotRR[0] );
	layout_PlotsRR->addWidget(plotRR[1]);
	layout_PlotsRR->addWidget(plotRR[2]);
	ui.displayGraphs_3->setLayout(layout_PlotsRR);
	//layout_PlotsEnt->addWidget( plotEnt_el1 );
	//layout_PlotsEnt->addWidget( plotEnt_el2 );
	//layout_PlotsEnt->addWidget( plotEnt_el3 );
	//ui.displayGraphs_6->setLayout(layout_PlotsEnt);

//----------
	msgBoxAbout = new QMessageBox(QMessageBox::Information,"About"," ",QMessageBox::Ok, this);
	msgBoxAbout->setText("SimpleHeart v0.9");
	msgBoxAbout->setInformativeText("Program wrtiien by \n Simple Heart Solutions \n\n  20.09.2012 \n\n  Cardiovascular Physics Group, WUT");
	msgBoxAbout->setStandardButtons(QMessageBox::Ok);
	msgBoxAbout->setDefaultButton(QMessageBox::Ok);
	msgBoxAbout->setDetailedText ( "podziemski@if.pw.edu.pl\nAll rights reserved" );


	ui.statusBar->showMessage(tr("Ready"));



}

void SimpleHeart::destroy()
{

}

void SimpleHeart::setupConnections()
{
	//---------------------CONTROLS CONNECTIONS -------------------------------------------
	//QObject::connect(this, SIGNAL(tajmer()), Machine2d, SLOT(processStep()));
	QObject::connect(ui.b_start, SIGNAL(clicked()),this, SLOT(startCalculation()));
	QObject::connect(ui.b_stop, SIGNAL(clicked()), this, SLOT(stopCalculation()));
	//QObject::connect(ui.b_stop, SIGNAL(clicked()), m_ioHandler, SLOT(getParametersFromFile()));

	QObject::connect(ui.b_reset, SIGNAL(clicked()), Machine2d, SLOT(reset()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), this, SLOT(reset()));


	for (short k = 0; k < Machine2d->stimulator->probeElectrodesCount(); ++k)
	{
		QObject::connect(ui.b_reset, SIGNAL(clicked()), Machine2d->stimulator->probeElectrode(k), SLOT(reset()));
	}
	for (short k = 0; k < plotPotentials.size(); ++k)
	{
		QObject::connect(ui.b_reset, SIGNAL(clicked()), plotPotentials[k], SLOT(clear()));
	}
	for (short k = 0; k < plotRR.size(); ++k)
	{
		QObject::connect(Machine2d->stimulator->probeElectrode(k), SIGNAL(nextRR(double)), plotRR[k], SLOT(appendRR(double)));
	}

	QObject::connect(ui.cb_saveBMP, SIGNAL(toggled()), this, SLOT(stopCalculation()));
	QObject::connect(ui.cb_saveBMP, SIGNAL(toggled(bool)), this, SLOT(setBmpSaving(bool)));
	QObject::connect(ui.b_snapShot, SIGNAL(clicked()), m_ioHandler, SLOT(stopCalculation()));
	QObject::connect(ui.b_snapShot, SIGNAL(clicked()), m_ioHandler, SLOT(setBmp()));
	QObject::connect(ui.b_snapShot, SIGNAL(clicked()), m_ioHandler, SLOT(saveAsBmp()));
	
	//ADDED

	QObject::connect(ui.b_saveStructure, SIGNAL(clicked()), m_ioHandler, SLOT(saveCurrentStructure()));
	QObject::connect(ui.b_loadStructure, SIGNAL(clicked()), this, SLOT(setAtrialStructure()));
	QObject::connect(ui.b_saveCurrentState, SIGNAL(clicked()), m_ioHandler, SLOT(saveCurrentState()));
	QObject::connect(ui.b_loadState, SIGNAL(clicked()), m_ioHandler, SLOT(loadCurrentState()));

	QObject::connect(ui.b_calculateFullElectrogram, SIGNAL(clicked()), Machine2d, SLOT(stopCalculation()));
	QObject::connect(ui.b_calculateFullElectrogram, SIGNAL(clicked()), Machine2d, SLOT(calculateFullElectrogramMap()));

	QObject::connect(ui.rb_setDisplayCsdMode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayCSD(bool)));
	QObject::connect(ui.rb_setDisplayPotentialMode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayPotential(bool)));
	QObject::connect(ui.rb_setDisplayC1Mode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayCurrent1(bool)));
	QObject::connect(ui.rb_setDisplayC2Mode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayCurrent2(bool)));
	QObject::connect(ui.rb_setDisplayActivationTime, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayActivationTime(bool)));


	QObject::connect(ui.rb_ATCsetRelativePhase, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setActivationTimeRelative(bool)));
	QObject::connect(ui.rb_ATCsetSynchronisedPhaseToS1, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setActivationTimeSynchronisedS1(bool)));
	QObject::connect(ui.rb_ATCsetSynchronisedPhaseToTCL, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setActivationTimeSynchronisedLastTCL(bool)));
	QObject::connect(ui.rb_ATCsetFixedPhase, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setActivationTimeFixed(bool)));

	QObject::connect(ui.b_ATCsetPhaseZero, SIGNAL(clicked()), this, SLOT(setActivationTimePhaseZero()));
	QObject::connect(ui.b_ATCtakeRangeFromLastTCL, SIGNAL(clicked()), this, SLOT(setActivationTimeCLlastTCL()));
	QObject::connect(ui.sb_ATCcycleLength, SIGNAL(valueChanged(double)), EpStimulator::Instance(), SLOT(setActivationTimeCycle(double)));
	QObject::connect(EpStimulator::Instance(), SIGNAL(progressOfStimulation(int)), ui.pb_pacingProtocol, SLOT(setValue(int)));
	QObject::connect(EpStimulator::Instance(), SIGNAL(progressOfSinglePace(int)), ui.pb_pacingProtocol_2, SLOT(setValue(int)));


	QObject::connect(ui.mode_tab, SIGNAL(currentChanged(int)), this, SLOT(setState(int)));

	QObject::connect(ui.b_setStateStructure, SIGNAL(toggled(bool)), glGraph, SLOT(setStateStructureModifier(bool)));
	QObject::connect(ui.b_setStatePaintERP, SIGNAL(toggled(bool)), glGraph, SLOT(setStateDiffusionModifier(bool)));
	QObject::connect(ui.b_setStatePaintConductivity, SIGNAL(toggled(bool)), glGraph, SLOT(setStateDiffusionModifier(bool)));
	QObject::connect(ui.b_setStatePaintERP, SIGNAL(toggled(bool)), this, SLOT(setPaintERP(bool)));
	QObject::connect(ui.b_setStatePaintConductivity, SIGNAL(toggled(bool)), this, SLOT(setPaintConductivity(bool)));



	QObject::connect(ui.b_freeTouchMode, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setModeFree(bool)));
	QObject::connect(ui.b_freeTouchMode, SIGNAL(toggled(bool)), this, SLOT(setFreeTouch(bool)));
	QObject::connect(ui.b_pacingModeFixed, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setModeFixedLoop(bool)));
	QObject::connect(ui.b_pacingModeSensing, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setModeSensing(bool)));


	QObject::connect(ui.sb_cycleLength_S1, SIGNAL(valueChanged(int)), EpStimulator::Instance(), SLOT(setCycleLength_S1(int)));
	QObject::connect(ui.sb_cycleLength_S3, SIGNAL(valueChanged(int)), EpStimulator::Instance(), SLOT(setCycleLength_S3(int)));
	QObject::connect(ui.sb_cycleLength_S2, SIGNAL(valueChanged(int)), EpStimulator::Instance(), SLOT(setCycleLength_S2(int)));
	QObject::connect(ui.sb_cycleCount_S1, SIGNAL(valueChanged(int)), EpStimulator::Instance(), SLOT(setNumberOfCycles_S1(int)));
	QObject::connect(ui.sb_cycleCount_S2, SIGNAL(valueChanged(int)), EpStimulator::Instance(), SLOT(setNumberOfCycles_S2(bool)));
	QObject::connect(ui.sb_cycleCount_S3, SIGNAL(valueChanged(int)), EpStimulator::Instance(), SLOT(setNumberOfCycles_S3(int)));
	QObject::connect(ui.cb_S2, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setS2On(bool)));
	QObject::connect(ui.cb_S3, SIGNAL(toggled(bool)), EpStimulator::Instance(), SLOT(setS3On(bool)));
	QObject::connect(ui.sb_couplingInterval, SIGNAL(valueChanged(int)), EpStimulator::Instance(), SLOT(setCouplingInterval(int)));

	QObject::connect(ui.b_startStimulationProcedure, SIGNAL(clicked()), Machine2d, SLOT(startStimulatorProcedure()));

	QObject::connect(ui.dial_paintValue, SIGNAL(valueChanged(int)), this, SLOT(setPaintValue(int)));
	QObject::connect(ui.sb_paintValue, SIGNAL(valueChanged(double)), this, SLOT(setPaintValue(double)));
	QObject::connect(ui.b_clearDiffusion, SIGNAL(clicked()), this, SLOT(paintUniform()));

	QObject::connect(ui.b_freeTouchMode, SIGNAL(toggled(bool)), this, SLOT(setFreeTouch(bool)));
/*
	QObject::connect(ui.b_pacingModeFree, SIGNAL(toggled(bool)), glGraph, SLOT(setStateViewer(bool)));
	QObject::connect(ui.b_pacingModeCycle, SIGNAL(toggled(bool)), glGraph, SLOT(setStateEP(bool)));
	QObject::connect(ui.b_pacingModeSensing, SIGNAL(toggled(bool)), glGraph, SLOT(setStateEP(bool)));
	QObject::connect(ui.b_pacingModeFixed SIGNAL(toggled(bool)), glGraph, SLOT(setStateEP(bool)));
	QObject::connect(ui.b_epStudy, SIGNAL(toggled(bool)), glGraph, SLOT(setStateEP(bool)));*/
	

//	QObject::connect(ui.b_diffPainting, SIGNAL(toggled(bool)), glGraph, SLOT(setStateDiffusionModifier(bool)));
//	QObject::connect(ui.b_stateViewer, SIGNAL(toggled(bool)), glGraph, SLOT(setStateViewer(bool)));
//	QObject::connect(ui.b_epStudy, SIGNAL(toggled(bool)), glGraph, SLOT(setStateEP(bool)));


	QObject::connect(ui.b_calculateFullElectrogram, SIGNAL(clicked()), glGraph, SLOT(displayElectrogram()));
	QObject::connect(ui.rb_setOutlineGauss, SIGNAL(toggled(bool)), glGraph, SLOT(setOutlineGauss(bool)));
	QObject::connect(ui.rb_setOutlineUniform, SIGNAL(toggled(bool)), glGraph, SLOT(setOutlineUniform(bool)));
	
	//ADDED END

	QObject::connect(ui.cb_calcEnt, SIGNAL(toggled(bool)), this, SLOT(setEntropyToggle(bool)));
	QObject::connect(ui.cb_calcPat, SIGNAL(toggled(bool)), this, SLOT(setEntropyPat(bool)));
	QObject::connect(ui.cb_calcVar, SIGNAL(toggled(bool)), this, SLOT(setEntropyVar(bool)));

//---------------------INDICATOR CONNECTIONS ------------------------------------------
	QObject::connect(this, SIGNAL(emitGlobalTimeOnly(double)), ui.lcd_time, SLOT(display(double)));


	//QObject::connect(ui.rb_paintDiffusion, SIGNAL(toggled(bool)), diffusionPainter, SLOT(setCurrentPainter_Diffusion()));
	//QObject::connect(ui.rb_paintAnisotrophy, SIGNAL(toggled(bool)), diffusionPainter, SLOT(setCurrentPainter_Anisotrophy()));
	
	//QObject::connect(ui.dial_diffSizeN, SIGNAL(valueChanged(int)), diffusionPainter, SLOT(setSigma(int)));
	//QObject::connect(ui.dial_diffValueN, SIGNAL(valueChanged(int)), diffusionPainter, SLOT(setAmplitude(int)));


	QObject::connect(ui.actionO_programie, SIGNAL(activated()), msgBoxAbout, SLOT(exec()));
	QObject::connect(ui.actionInstrukcja, SIGNAL(activated()), this, SLOT(openInstruction()));

		
	QObject::connect(ui.actionSettings, SIGNAL(activated()), this, SLOT(openSettings()));

//---------------------GRAPH CONNECTIONS -------------------------------------------
	//QObject::connect(diffusionPainter, SIGNAL(positionElektrode(int, int,int)), this, SLOT(setProbeElectrode(int,int,int)));

	for (short k = 0; k < Machine2d->stimulator->probeElectrodesCount(); ++k)
	{
		QObject::connect(Machine2d->stimulator->probeElectrode(k), SIGNAL(newElectrogramAndTime(double, double)), plotPotentials[k], SLOT(addValue_e1(double, double)));
	}

	for (short k = 0; k < Machine2d->stimulator->probeElectrodesCount(); ++k)
	{
		QObject::connect(Machine2d->stimulator->probeElectrode(k), SIGNAL(nextRR(double)), plotRR[k], SLOT(appendRR(double)));
	}


	QObject::connect(ui.b_potentialplot_clear, SIGNAL(clicked()), this, SLOT(plotPotentialClear()));
	for (short k = 0; k < plotPotentials.size(); ++k)
	{
		QObject::connect(ui.b_potentialplot_autoscale, SIGNAL(clicked()), plotPotentials[k], SLOT(autoscale()));
		QObject::connect(ui.b_potentialplot_zoominY, SIGNAL(clicked()), plotPotentials[k], SLOT(zoomInY()));
		QObject::connect(ui.b_potentialplot_zoomoutY, SIGNAL(clicked()), plotPotentials[k], SLOT(zoomOutY()));
		QObject::connect(ui.b_potentialplot_zoominX, SIGNAL(clicked()), plotPotentials[k], SLOT(zoomInX()));
		QObject::connect(ui.b_potentialplot_zoomoutX, SIGNAL(clicked()), plotPotentials[k], SLOT(zoomOutX()));
	}
	QObject::connect(ui.b_potentialplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(savePotentialPlot()));

	QObject::connect(ui.b_rrplot_clear, SIGNAL(clicked()), this, SLOT(plotRRClear()));
	for (short k = 0; k < plotRR.size(); ++k)
	{

		QObject::connect(ui.b_rrplot_autoscale, SIGNAL(clicked()), plotRR[k], SLOT(autoscale()));
		QObject::connect(ui.b_rrplot_zoominX, SIGNAL(clicked()), plotRR[k], SLOT(zoomInX()));
		QObject::connect(ui.b_rrplot_zoomoutX, SIGNAL(clicked()), plotRR[k], SLOT(zoomOutX()));
		QObject::connect(ui.sb_yminRR, SIGNAL(valueChanged(double)), plotRR[k], SLOT(setYmin(double)));
		QObject::connect(ui.sb_ymaxRR, SIGNAL(valueChanged(double)), plotRR[k], SLOT(setYmax(double)));
	}
	QObject::connect(ui.b_rrlplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(saveRRPlot_1()));
	QObject::connect(ui.b_rrlplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(saveRRPlot_2()));
	QObject::connect(ui.b_rrlplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(saveRRPlot_3()));
}


void SimpleHeart::setActivationTimePhaseZero()
{
	double val = Machine2d->m_globalTime;
	ui.sb_ATCphaseZero->setValue(val);
	EpStimulator::Instance()->setPhaseZero(ui.sb_ATCphaseZero->value());
}
void SimpleHeart::setActivationTimeCLlastTCL()
{
	double val = EpStimulator::Instance()->probeElectrode(0)->lastRR();
	ui.sb_ATCcycleLength->setValue(val);
	EpStimulator::Instance()->setActivationTimeCycle(val);
}
void SimpleHeart::paintUniform()
{
	if (!QMessageBox::question(
		this,
		tr("Simple Heart"),
		tr("Do you want to make current painting uniform?"),
		tr("&Yes"), tr("&No"),
		QString::null, 0, 1))
	{

		if (ui.b_setStatePaintConductivity->isChecked())
		{
			Machine2d->setUniformDiffusion(glGraph->paintValueDiffusion);

		}
		else if (ui.b_setStatePaintERP->isChecked())
		{
			Machine2d->setUniformERP(glGraph->paintValueERP);
		}
	}

}
void SimpleHeart::setPaintERP(bool b)
{
	if (b)
	{
		QObject::disconnect(ui.sb_paintValue, SIGNAL(valueChanged(double)), this, SLOT(setPaintValue(double)));
		QObject::disconnect(ui.dial_paintValue, SIGNAL(valueChanged(int)), this, SLOT(setPaintValue(int)));
		glGraph->setPaintERP(b);
		ui.sb_paintValue->setMaximum(m_grid->maxERP);
		ui.sb_paintValue->setMinimum(m_grid->minERP);
		ui.sb_paintValue->setValue(glGraph->paintValueERP);
		ui.l_maxPaintValue->setText(QString::number(m_grid->maxERP));
		ui.l_minPaintValue->setText(QString::number(m_grid->minERP));

		int val = floor(ui.dial_paintValue->maximum() * (glGraph->paintValueERP - m_grid->minERP) / (m_grid->maxERP - m_grid->minERP));
		ui.dial_paintValue->setValue(val);
		QObject::connect(ui.sb_paintValue, SIGNAL(valueChanged(double)), this, SLOT(setPaintValue(double)));
		QObject::connect(ui.dial_paintValue, SIGNAL(valueChanged(int)), this, SLOT(setPaintValue(int)));
	}

}
void SimpleHeart::setPaintConductivity(bool b)
{
	if (b)
	{
		QObject::disconnect(ui.sb_paintValue, SIGNAL(valueChanged(double)), this, SLOT(setPaintValue(double)));
		QObject::disconnect(ui.dial_paintValue, SIGNAL(valueChanged(int)), this, SLOT(setPaintValue(int)));
		glGraph->setPaintConductivity(b);
		ui.sb_paintValue->setMaximum(m_grid->maxDiffusion);
		ui.sb_paintValue->setMinimum(m_grid->minDiffusion);
		ui.sb_paintValue->setValue(glGraph->paintValueDiffusion);
		ui.l_maxPaintValue->setText(QString::number(m_grid->maxDiffusion));
		ui.l_minPaintValue->setText(QString::number(m_grid->minDiffusion));
		
		int val = floor(ui.dial_paintValue->maximum() * (glGraph->paintValueDiffusion - m_grid->minDiffusion) / (m_grid->maxDiffusion - m_grid->minDiffusion));
		ui.dial_paintValue->setValue(val);
		QObject::connect(ui.sb_paintValue, SIGNAL(valueChanged(double)), this, SLOT(setPaintValue(double)));
		QObject::connect(ui.dial_paintValue, SIGNAL(valueChanged(int)), this, SLOT(setPaintValue(int)));
	}

}
void SimpleHeart::setPaintValue(int val)
{
	QObject::disconnect(ui.sb_paintValue, SIGNAL(valueChanged(double)), this, SLOT(setPaintValue(double)));
	if (ui.b_setStatePaintConductivity->isChecked())
	{
		double setval = (static_cast<double>(val) / ui.dial_paintValue->maximum())* (m_grid->maxDiffusion - m_grid->minDiffusion) + m_grid->minDiffusion;
		ui.sb_paintValue->setValue(setval);
		glGraph->paintValueDiffusion = setval;

	}
	else if (ui.b_setStatePaintERP->isChecked())
	{
		double setval = (static_cast<double>(val) / ui.dial_paintValue->maximum())* (m_grid->maxERP - m_grid->minERP) + m_grid->minERP;
		ui.sb_paintValue->setValue(setval);
		glGraph->paintValueERP = setval;
	}
	QObject::connect(ui.sb_paintValue, SIGNAL(valueChanged(double)), this, SLOT(setPaintValue(double)));
}
void SimpleHeart::setPaintValue(double val)
{
	QObject::disconnect(ui.dial_paintValue, SIGNAL(valueChanged(int)), this, SLOT(setPaintValue(int)));
	if (ui.b_setStatePaintConductivity->isChecked())
	{
		glGraph->paintValueDiffusion = val;
		int setval = floor(ui.dial_paintValue->maximum() * (glGraph->paintValueDiffusion - m_grid->minDiffusion) / (m_grid->maxDiffusion - m_grid->minDiffusion));

		ui.dial_paintValue->setValue(setval);
	}
	else if (ui.b_setStatePaintERP->isChecked())
	{
		glGraph->paintValueERP = val;
		int setval = floor(ui.dial_paintValue->maximum() * (glGraph->paintValueDiffusion - m_grid->minERP) / (m_grid->maxERP - m_grid->minERP));
		ui.dial_paintValue->setValue(setval);
	}
	QObject::connect(ui.dial_paintValue, SIGNAL(valueChanged(int)), this, SLOT(setPaintValue(int)));
}
void SimpleHeart::setFreeTouch(bool val)
{
	if (val)
	{
		glGraph->setStateViewer(true);

	}
	else
	{
		glGraph->setStateEP(true);
	}

}
void SimpleHeart::setState(int val)
{
	switch (val)
	{
	case 1:
		if (ui.b_freeTouchMode->isChecked()){
			glGraph->setStateViewer(true);

		} 
		else {
			glGraph->setStateEP(true);
		}
		break;
	case 2:
		glGraph->setStateDiffusionModifier(true);
		ui.b_setStateStructure->setChecked(false);
		ui.b_setStatePaintConductivity->setChecked(true);
		setPaintConductivity(true);
		break;
	}

}
void SimpleHeart::setEctopicPoint()
{
	Machine2d->m_definitions->m_ectopicSizeX = 3;
	Machine2d->m_definitions->m_ectopicSizeY = 3;
	ui.statusBar->showMessage(tr("Point Stimulation set"));
}
void SimpleHeart::setEctopicHoriz()
{
	Machine2d->m_definitions->m_ectopicSizeX =40;
	Machine2d->m_definitions->m_ectopicSizeY =2;
	ui.statusBar->showMessage(tr("Horizontal Stimulation set"));

}
void SimpleHeart::setEctopicVert()
{
	Machine2d->m_definitions->m_ectopicSizeX =2;
	Machine2d->m_definitions->m_ectopicSizeY =40;
	ui.statusBar->showMessage(tr("Vertical Stimulation set"));

}
void SimpleHeart::plotPotentialClear()
{
	if ( !QMessageBox::question(
            this,
            tr("SimpleHeart"),
            tr("Delete all data from graph without saving?"),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
	{
		for (short k = 0; k < plotPotentials.size(); ++k)
			plotPotentials[k]->clear();

		ui.statusBar->showMessage(tr("Plots cleared"));
	}

}
void SimpleHeart::plotRRClear()
{
	if ( !QMessageBox::question(
            this,
            tr("SimpleHeart"),
            tr("Delete all data from graph without saving?"),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
	{
		for (short k = 0; k < plotPotentials.size(); ++k)
			plotRR[k]->clear();

		plotRR[0]->d_curve.front()->setPen(QPen(QBrush(QColor(255,255,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
		plotRR[1]->d_curve.front()->setPen(QPen(QBrush(QColor(255,0,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
		plotRR[2]->d_curve.front()->setPen(QPen(QBrush(QColor(0,255,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
		ui.statusBar->showMessage(tr("Plots cleared"));
	}

}
void SimpleHeart::plotEntClear()
{
	if ( !QMessageBox::question(
            this,
            tr("SimpleHeart"),
            tr("Delete all data from graph without saving?"),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
	{
		plotEnt_el1->clear();	
		plotEnt_el1->d_curve.back()->setPen(QPen(Qt::yellow));
		plotEnt_el2->clear();
		plotEnt_el2->d_curve.back()->setPen(QPen(Qt::red));
		plotEnt_el3->clear();
		plotEnt_el3->d_curve.back()->setPen(QPen(Qt::green));
		ui.statusBar->showMessage(tr("Plots cleared"));
	}

}
//------------------------------------------------------------------
//  Generate new values 
void SimpleHeart::timerEvent(QTimerEvent *)
{
	//if(ui.cb_saveBMP->isChecked())
	//{
	//	m_ioHandler->saveAsBmp();
	//}
	double timee = Machine2d->m_globalTime;
	refreshCounter = (Machine2d->processStep() - timee);

	emit emitGlobalTimeOnly(Machine2d->m_globalTime);

	glGraph->updateGL();

	if (refreshCounter > refreshRate)
	{
		refreshCounter = 0.0;

	/*	m_grid->m_minElectrogram = m_grid->m_minPotential;
		m_grid->m_maxElectrogram = m_grid->m_maxPotential;
		if (simpleParameters->m_ectopicActivity && m_grid->stimulationBegun == false)
		{
			m_grid->startStimulation(m_grid->m_mesh[m_grid->m_stimulationID], m_grid->m_stimulationID, 2, 0.1);
		}

		else if (!simpleParameters->m_ectopicActivity && m_grid->stimulationBegun == true)
		{
			m_grid->stopStimulation();
		}

		Machine2d->RRcalc_1->processNewTime(Machine2d->m_globalTime, Machine2d->probeOscillator[0]->getPotential());
		Machine2d->RRcalc_2->processNewTime(Machine2d->m_globalTime, Machine2d->probeOscillator[1]->getPotential());
		Machine2d->RRcalc_3->processNewTime(Machine2d->m_globalTime, Machine2d->probeOscillator[2]->getPotential());
		m_grid->calculateElectrogram(Machine2d->probeOscillator[0]);
		m_grid->calculateElectrogram(Machine2d->probeOscillator[1]);
		m_grid->calculateElectrogram(Machine2d->probeOscillator[2]);
		Machine2d->probeOscillator[0]->stateCalculated(1, 0, 0);
		Machine2d->probeOscillator[1]->stateCalculated(1, 0, 0);
		Machine2d->probeOscillator[2]->stateCalculated(1, 0, 0);

		emit emitGlobalTimeOnly(Machine2d->m_globalTime);*/

	}
	//emit tajmer();


}

void SimpleHeart::startCalculation()
{
	setTimerInterval(0);
	m_calculationsArePaused = false;
	m_calculationsAreRunning = true;
	ui.statusBar->showMessage("calculating...");

}
void SimpleHeart::stopCalculation()
{
	//Machine2d->calculateElectrogram();
	Machine2d->m_strategy->synchronise();
	glGraph->updateGL();
	setTimerInterval(-1);	
	m_calculationsAreRunning = false;
	m_calculationsArePaused = false;
	ui.statusBar->showMessage("calculations paused");
}
void SimpleHeart::resetDiffusion()
{
	if ( !QMessageBox::question(
            this,
            tr("Simple Heart"),
            tr("Do you want to reset current diffusion and anisotropy ?"),
            tr("&Yes"), tr("&No"),
            QString::null, 0, 1 ) )
	{

		//diffusionPainter->m_pixelmap->resetDefault(255,255,255);
		//diffusionPainter->m_anisotrophy->resetDefault(150,150,0);
		//diffusionPainter->updateGL();
		setAtrialDiffusion();
		///TODO tu coœ z elektrodami
	}
}
void SimpleHeart::setAtrialDiffusion()
{
	//for (unsigned int j = 0; j < m_grid->getSize(); ++j)
	//{
	//	for (unsigned int i = 0; i < m_grid->getSize(); ++i)
	//	{
	//		Machine2d->m_diffusionCoefficients[j][i] = diffusionPainter->m_pixelmap->m_matrix[j][i]->color.greenF()*(diffusionPainter->m_upperLimit - diffusionPainter->m_lowerLimit) + diffusionPainter->m_lowerLimit;
	//		Machine2d->m_anisotrophyCoefficients[j][i] = 0.5;// diffusionPainter->m_anisotrophy->m_matrix[j][i]->color.redF();
	//	
	//}
	//}
	//Machine2d->editDiffusionCoefficients();
	//ui.statusBar->showMessage("Diffusion set");


}

void SimpleHeart::setEntropyToggle(bool v)
{

	//ui.statusBar->showMessage(tr("setEntropyToggle"));
	//if(v)
	//{
	//	ui.statusBar->showMessage("Entropy measurment ON");
	//	if(m_patternEnt)
	//	{
	//		QObject::connect( Machine2d->RRcalc_1, SIGNAL(nextPatEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
	//		QObject::connect( Machine2d->RRcalc_2, SIGNAL(nextPatEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
	//		QObject::connect( Machine2d->RRcalc_3, SIGNAL(nextPatEn(double)),plotEnt_el3, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextVarEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextVarEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextVarEn(double)),plotEnt_el3, SLOT(appendRR(double)));	

	//		
	//		QObject::connect(Machine2d->RRcalc_1, SIGNAL(nextRR(double)), Machine2d->RRcalc_1, SLOT(calculatePatEn(double)));
	//		QObject::connect(Machine2d->RRcalc_2, SIGNAL(nextRR(double)), Machine2d->RRcalc_2, SLOT(calculatePatEn(double)));
	//		QObject::connect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculatePatEn(double)));
	//		QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR(double)), Machine2d->RRcalc_2, SLOT(calculateVarEn(double)));
	//		QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
	//		QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
	//	}
	//	if(m_variabilityEnt)
	//	{
	//		QObject::connect( Machine2d->RRcalc_1, SIGNAL(nextVarEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
	//		QObject::connect( Machine2d->RRcalc_2, SIGNAL(nextVarEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
	//		QObject::connect( Machine2d->RRcalc_3, SIGNAL(nextVarEn(double)),plotEnt_el3, SLOT(appendRR(double)));
	//		QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextPatEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextPatEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextPatEn(double)),plotEnt_el3, SLOT(appendRR(double)));	

	//
	//		QObject::disconnect(Machine2d->RRcalc_1, SIGNAL(nextRR()), Machine2d->RRcalc_1, SLOT(calculatePatEn()));
	//		QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR()), Machine2d->RRcalc_2, SLOT(calculatePatEn()));
	//		QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR()), Machine2d->RRcalc_3, SLOT(calculatePatEn()));
	//		QObject::connect(Machine2d->RRcalc_1, SIGNAL(nextRR()), Machine2d->RRcalc_1, SLOT(calculateVarEn()));
	//		QObject::connect(Machine2d->RRcalc_2, SIGNAL(nextRR()), Machine2d->RRcalc_2, SLOT(calculateVarEn()));
	//		QObject::connect(Machine2d->RRcalc_3, SIGNAL(nextRR()), Machine2d->RRcalc_3, SLOT(calculateVarEn()));
	//
	//	}
	//}
	//else
	//{
	//	ui.statusBar->showMessage("Entropy measurment OFF");
	//		QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextPatEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextPatEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextPatEn(double)),plotEnt_el3, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextVarEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextVarEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
	//		QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextVarEn(double)),plotEnt_el3, SLOT(appendRR(double)));	
	//		QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR(double)), Machine2d->RRcalc_2, SLOT(calculateVarEn(double)));
	//		QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
	//		QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
	//		//QObject::disconnect(Machine2d->RRcalc_1, SIGNAL(nextRR()), Machine2d->RRcalc_1, SLOT(calculatePatEn()));
	//		//QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR()), Machine2d->RRcalc_2, SLOT(calculatePatEn()));
	//		//QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR()), Machine2d->RRcalc_3, SLOT(calculatePatEn()));
	//}
}
void SimpleHeart::setEntropyPat(bool v)
{
	m_patternEnt = true;
	m_variabilityEnt = false;

	setEntropyToggle(ui.cb_calcEnt->isChecked());
	ui.statusBar->showMessage(tr("Pattern Entropy Chosen"));
}
void SimpleHeart::setEntropyVar(bool v)
{
	m_patternEnt = false;
	m_variabilityEnt = true;
	
	ui.statusBar->showMessage(tr("Variability Entropy Chosen"));
}

void SimpleHeart::setEctopicModS1(bool b)
{
	Machine2d->m_definitions->m_ectopicS1_toggle = b;
	if(b)
	{
		ui.statusBar->showMessage(tr("S1 stimulation ON"));
	}
	else
		ui.statusBar->showMessage(tr("S1 stimulation OFF"));
}
void SimpleHeart::setEctopicPeriodS1(double v)
{
	Machine2d->m_definitions->m_ectopicPeriodS1 = v;
}

void SimpleHeart::setEctopicModS2(bool b)
{
	Machine2d->m_definitions->m_ectopicS2_toggle = b;
	if(b)
	{
		ui.statusBar->showMessage(tr("S2 stimulation ON"));
	}
	else
		ui.statusBar->showMessage(tr("S2 stimulation OFF"));
}
void SimpleHeart::setEctopicPeriodS2(double v)
{
	Machine2d->m_definitions->m_ectopicPeriodS2 = v;
}
void SimpleHeart::setAtrialRestitution(int value)
{
	//Machine2d->setAtriumGlobalNi(static_cast<double>(((100-value)))/static_cast<double>(1000));
	
	//Machine2d->setAtriumV3multi(static_cast<double>(((100-value)))/static_cast<double>(100));
	
	//Machine2d->setAtriumGlobalNi(static_cast<double>(((100-value)))/static_cast<double>(1000));
}
//-------------------------------------------------------------------------
void SimpleHeart::setBmpSaving(bool t)
{
	if(ui.cb_saveBMP->isChecked())
	{
		m_ioHandler->setBmp();

		ui.statusBar->showMessage(tr("BMP saving ON"));
	}
	else
		ui.statusBar->showMessage(tr("BMP saving OFF"));
}
void SimpleHeart::setAtrialStructure()
{
	stopCalculation();

	for (short k = 0; k < Machine2d->stimulator->probeElectrodesCount(); ++k)
	{
		QObject::disconnect(Machine2d->stimulator->probeElectrode(k), SIGNAL(newElectrogramAndTime(double, double)), plotPotentials[k], SLOT(addValue_e1(double, double)));
	}
	
	glGraph->updateGL();

	CardiacMesh *ptr = m_ioHandler->loadCustomStructure();
	if (ptr != nullptr)
	{
		CardiacMesh *tempptr = m_grid;
		m_grid = ptr;

		Machine2d->m_grid = m_grid;

		(Machine2d->*Machine2d->setStrategy)();
		glGraph->linkToMesh = m_grid;

		Machine2d->stimulator->setProbeElectrode(m_grid, 0, 129);
		Machine2d->stimulator->setProbeElectrode(m_grid, 1, 16257 - 128);
		Machine2d->stimulator->setProbeElectrode(m_grid, 2, 5000);

		Machine2d->reset();
		delete(tempptr);
	}

	for (short k = 0; k < Machine2d->stimulator->probeElectrodesCount(); ++k)
	{
		QObject::connect(Machine2d->stimulator->probeElectrode(k), SIGNAL(newElectrogramAndTime(double, double)), plotPotentials[k], SLOT(addValue_e1(double, double)));
	}

//		m_ioHandler->writeParametersToFile();
//
//	 	QObject::disconnect( Machine2d->probeOscillator[0], SIGNAL(newPotentialTime(double,double)),plotPotentialE1, SLOT(addValue_e1(double,double)));
//		QObject::disconnect( Machine2d->probeOscillator[1], SIGNAL(newPotentialTime(double,double)),plotPotentialE2, SLOT(addValue_e2(double,double)));
//		QObject::disconnect( Machine2d->probeOscillator[2], SIGNAL(newPotentialTime(double,double)),plotPotentialE3, SLOT(addValue_e3(double,double)));
//	
//
//
//	glGraph->updateGL();
//
///*
//		m_ioHandler->readStructureFromFile(ui.cb_setStructure->currentIndex());
//		Machine2d->setAtrialStructure();
//		///TODO tu coœ z elektrodami
//
//
//
//		for (unsigned int j = 0; j < Machine2d->m_oscillators.size(); ++j)
//		{
//			for (unsigned int i = 0; i <  Machine2d->m_oscillators[j].size(); ++i)
//			{
//				Machine2d->m_diffusionCoefficients[j][i] = 1.0*(diffusionPainter->m_upperLimit - diffusionPainter->m_lowerLimit) + diffusionPainter->m_lowerLimit;
//				Machine2d->m_anisotrophyCoefficients[j][i] = 0.5;
//				QColor colorek  = diffusionPainter->m_pixelmap->m_matrix[i][j]->color;
//				colorek.setRgbF(1.0,1.0,1.0);
//				diffusionPainter->m_pixelmap->m_matrix[i][j]->color = colorek;
//				colorek.setRgbF(0.5,0.5,0.0);
//				diffusionPainter->m_anisotrophy->m_matrix[i][j]->color = colorek;
//			}
//		}
//*/
//		Machine2d->editDiffusionCoefficients();
//
//		diffusionPainter->updateCanvas();
//		diffusionPainter->updateGL();
///*
//		Machine2d->probeOscillator[0] =  Machine2d->m_oscillators[diffusionPainter->probe1->position().x()][diffusionPainter->probe1->myPosition.y()];
//		Machine2d->probeOscillator[1] =  Machine2d->m_oscillators[diffusionPainter->probe2->position().x()][diffusionPainter->probe2->myPosition.y()];
//		Machine2d->probeOscillator[2] = Machine2d->m_oscillators[diffusionPainter->probe3->position().x()][diffusionPainter->probe3->myPosition.y()];*/
//		Machine2d->probeOscillator[0] = m_grid->m_mesh[m_grid->getSize()*diffusionPainter->probe1->position().x() + diffusionPainter->probe1->myPosition.y()];
//		Machine2d->probeOscillator[1] = m_grid->m_mesh[m_grid->getSize()*diffusionPainter->probe2->position().x() + diffusionPainter->probe2->myPosition.y()];
//		Machine2d->probeOscillator[2] = m_grid->m_mesh[m_grid->getSize()*diffusionPainter->probe3->position().x() + diffusionPainter->probe3->myPosition.y()];
//
//
//	 QObject::connect( Machine2d->probeOscillator[0], SIGNAL(newPotentialTime(double,double)),plotPotentialE1, SLOT(addValue_e1(double,double)));
//	 QObject::connect( Machine2d->probeOscillator[1], SIGNAL(newPotentialTime(double,double)),plotPotentialE2, SLOT(addValue_e2(double,double)));
//	 QObject::connect( Machine2d->probeOscillator[2], SIGNAL(newPotentialTime(double,double)),plotPotentialE3, SLOT(addValue_e3(double,double)));
//		
//	 m_ioHandler->getParametersFromFile();
}


void SimpleHeart::setTimerInterval(double ms)
{
    d_interval = qRound(ms);

    if ( d_timerId >= 0 )
    {
        killTimer(d_timerId);
        d_timerId = -1;
    }
    if (d_interval >= 0 )
        d_timerId = startTimer(d_interval);
}

void SimpleHeart::openInstruction()
{
	 QString str =	QDir::currentPath();
	 str.prepend("file:///");
	 str.append("/InstrukcjaSimpleHeart.pdf");
	 QDesktopServices::openUrl ( QUrl(str, QUrl::TolerantMode) );
}

void SimpleHeart::openSettings()
{
	 QString str =	QDir::currentPath();
	 str.prepend("file:///");
	 str.append("/parameters.inf");
	 QDesktopServices::openUrl ( QUrl(str, QUrl::TolerantMode) );
	 m_ioHandler->getParametersFromFile();
}