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
	m_patternEnt = true;
	m_variabilityEnt = false;
	setEntropyPat(m_patternEnt);
	setAtrialStructure();
	ui.statusBar->showMessage(tr("Ready"));
}

SimpleHeart::~SimpleHeart()
{
	m_ioHandler->writeParametersToFile();
}

void SimpleHeart::reset()
{
	ui.statusBar->showMessage(tr("Simulation reset"));

	QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextRR(double)),plotRR_el1, SLOT(appendRR(double)));	
	QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextRR(double)),plotRR_el2, SLOT(appendRR(double)));	
	QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextRR(double)),plotRR_el3, SLOT(appendRR(double)));	
	plotRR_el1->clear();
	plotRR_el1->d_curve.front()->setPen(QPen(QBrush(QColor(255,255,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR_el2->clear();
	plotRR_el2->d_curve.back()->setPen(QPen(QBrush(QColor(255,0,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR_el3->clear();
	plotRR_el3->d_curve.back()->setPen(QPen(QBrush(QColor(0,255,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
	ui.statusBar->showMessage(tr("Simulation reset"));
	QObject::connect( Machine2d->RRcalc_1, SIGNAL(nextRR(double)),plotRR_el1, SLOT(appendRR(double)));	
	QObject::connect( Machine2d->RRcalc_2, SIGNAL(nextRR(double)),plotRR_el2, SLOT(appendRR(double)));	
	QObject::connect( Machine2d->RRcalc_3, SIGNAL(nextRR(double)),plotRR_el3, SLOT(appendRR(double)));	

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
	plotPotentialE1 = new TimePlot(ui.displayGraphs);
	plotPotentialE1->setObjectName(QString::fromUtf8("Electrogram 1"));
	//plotPotentialE1->setAxisAutoScale( QwtPlot::yLeft );
	plotPotentialE1->setMinimumHeight(2);
	plotPotentialE1->enableAxis(QwtPlot::xBottom, false);
	plotPotentialE1->setAxisTitle(QwtPlot::yLeft, "Probe 1");
	plotPotentialE2 = new TimePlot(ui.displayGraphs);
	plotPotentialE2->setObjectName(QString::fromUtf8("Electrogram 2"));
	plotPotentialE2->setAxisTitle(QwtPlot::yLeft, "Probe 2");
	//	plotPotentialE2->setAxisAutoScale( QwtPlot::yLeft );
	plotPotentialE2->setMinimumHeight(2);
	plotPotentialE2->enableAxis(QwtPlot::xBottom, false);
	plotPotentialE3 = new TimePlot(ui.displayGraphs);
	plotPotentialE3->setObjectName(QString::fromUtf8("Electrogram 3"));
	plotPotentialE3->setAxisTitle(QwtPlot::yLeft, "Probe 3");
	//plotPotentialE3->setAxisAutoScale( QwtPlot::yLeft );
	plotPotentialE3->setMinimumHeight(2);
	//------------------------------
	plotRR_el1 = new rrPlot(ui.displayGraphs_3);
	plotRR_el1->setObjectName(QString::fromUtf8("plot RR"));
	plotRR_el1->setAxisAutoScale(QwtPlot::yLeft);
	plotRR_el1->setAxisScale(QwtPlot::xBottom, 0, 99);
	plotRR_el1->setMinimumHeight(2);

	plotRR_el1->d_curve.front()->setPen(QPen(QBrush(QColor(255, 255, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR_el2 = new rrPlot(ui.displayGraphs_3);
	plotRR_el2->setObjectName(QString::fromUtf8("plot RR"));
	plotRR_el2->setAxisAutoScale(QwtPlot::yLeft);
	plotRR_el2->setAxisScale(QwtPlot::xBottom, 0, 99);
	plotRR_el2->setMinimumHeight(2);
	plotRR_el2->d_curve.front()->setPen(QPen(QBrush(QColor(255, 0, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	plotRR_el3 = new rrPlot(ui.displayGraphs_3);
	plotRR_el3->setObjectName(QString::fromUtf8("plot RR"));
	plotRR_el3->setAxisAutoScale(QwtPlot::yLeft);
	plotRR_el3->setAxisScale(QwtPlot::xBottom, 0, 99);
	plotRR_el3->setMinimumHeight(2);
	plotRR_el3->d_curve.front()->setPen(QPen(QBrush(QColor(0, 255, 0), Qt::SolidPattern), 2.0, Qt::SolidLine));
	//------------------------------
	plotEnt_el1 = new rrPlot(ui.displayGraphs_3);
	plotEnt_el1->setObjectName(QString::fromUtf8("plot Entropy"));
	plotEnt_el1->setAxisAutoScale(QwtPlot::yLeft);
	plotEnt_el1->setAxisScale(QwtPlot::xBottom, 0, 99);
	plotEnt_el1->setMinimumHeight(2);

	plotEnt_el1->d_curve.back()->setPen(QPen(Qt::yellow));
	plotEnt_el2 = new rrPlot(ui.displayGraphs_3);
	plotEnt_el2->setObjectName(QString::fromUtf8("plot Entropy"));
	plotEnt_el2->setAxisAutoScale(QwtPlot::yLeft);
	plotEnt_el2->setAxisScale(QwtPlot::xBottom, 0, 99);
	plotEnt_el2->setMinimumHeight(2);
	plotEnt_el2->d_curve.back()->setPen(QPen(Qt::red));
	plotEnt_el3 = new rrPlot(ui.displayGraphs_3);
	plotEnt_el3->setObjectName(QString::fromUtf8("plot Entropy"));
	plotEnt_el3->setAxisAutoScale(QwtPlot::yLeft);
	plotEnt_el3->setAxisScale(QwtPlot::xBottom, 0, 99);
	plotEnt_el3->setMinimumHeight(2);
	plotEnt_el3->d_curve.back()->setPen(QPen(Qt::green));













	simpleParameters = new atrialParameters();
	m_grid = CardiacMesh::constructCartesianGrid(100,100, 0.4, 0.4, ATRIAL_V3);
	//m_grid = new CartesianGrid(256,256,0.05,0.05);
	m_matrix = new DiffusionMatrix(m_grid);
	m_anisotrophy = new DiffusionMatrix(m_grid);

	Machine2d = new AtrialMachine2d(simpleParameters, m_grid);
	glGraph = new glAtrium(m_grid,Machine2d, ui.displayMain);

	diffusionPainter = new DiffusionPainter(m_grid, m_matrix,m_anisotrophy, ui.displayDiffusion);
	

	
//vis tab-----------------------------
	layout_Visualisation->addWidget( glGraph );
	ui.displayMain->setLayout(layout_Visualisation);
	layout_Diffusion->addWidget( diffusionPainter );
	ui.displayDiffusion->setLayout(layout_Diffusion);
	layout_Plots->addWidget( plotPotentialE1 );
	layout_Plots->addWidget( plotPotentialE2 );
	layout_Plots->addWidget( plotPotentialE3 );
	ui.displayGraphs->setLayout(layout_Plots);
	layout_PlotsRR->addWidget( plotRR_el1 );
	layout_PlotsRR->addWidget( plotRR_el2 );
	layout_PlotsRR->addWidget( plotRR_el3 );
	ui.displayGraphs_3->setLayout(layout_PlotsRR);
	layout_PlotsEnt->addWidget( plotEnt_el1 );
	layout_PlotsEnt->addWidget( plotEnt_el2 );
	layout_PlotsEnt->addWidget( plotEnt_el3 );
	ui.displayGraphs_6->setLayout(layout_PlotsEnt);

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
	QObject::connect(this, SIGNAL(tajmer()), Machine2d, SLOT(processStep()));
	QObject::connect(ui.b_start, SIGNAL(clicked()),this, SLOT(startCalculation()));
	QObject::connect(ui.b_stop, SIGNAL(clicked()), this, SLOT(stopCalculation()));
	//QObject::connect(ui.b_stop, SIGNAL(clicked()), m_ioHandler, SLOT(getParametersFromFile()));

	QObject::connect(ui.b_reset, SIGNAL(clicked()), Machine2d, SLOT(reset()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), this, SLOT(reset()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), plotPotentialE1, SLOT(clear()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), plotPotentialE2, SLOT(clear()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), plotPotentialE3, SLOT(clear()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), Machine2d->RRcalc_1, SLOT(reset()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), Machine2d->RRcalc_2, SLOT(reset()));
	QObject::connect(ui.b_reset, SIGNAL(clicked()), Machine2d->RRcalc_3, SLOT(reset()));

	QObject::connect(ui.b_calculateFullElectrogram, SIGNAL(clicked()), Machine2d, SLOT(stopCalculation()));
	QObject::connect(ui.b_calculateFullElectrogram, SIGNAL(clicked()), Machine2d, SLOT(calculateFullElectrogramMap()));
//	QObject::connect(ui.sb_tau, SIGNAL(valueChanged(int)), Machine2d->m_definitions, SLOT(setTau(int)));
//	QObject::connect(ui.sb_winSize, SIGNAL(valueChanged(int)), Machine2d->m_definitions, SLOT(setWindowSize(int)));
//	QObject::connect(ui.sb_binSize, SIGNAL(valueChanged(double)), Machine2d->m_definitions, SLOT(setBinSize(double)));
	

	QObject::connect(ui.cb_saveBMP, SIGNAL(toggled()), this, SLOT(stopCalculation()));
	QObject::connect(ui.cb_saveBMP, SIGNAL(toggled(bool)), this, SLOT(setBmpSaving(bool)));
	QObject::connect(ui.sb_simPeriod, SIGNAL(valueChanged(double)), this, SLOT(setEctopicPeriodS1(double)));
	QObject::connect(ui.cb_simPeriod, SIGNAL(toggled(bool)), this, SLOT(setEctopicModS1(bool)));
	QObject::connect(ui.sb_simPeriod_2, SIGNAL(valueChanged(double)), this, SLOT(setEctopicPeriodS2(double)));
	QObject::connect(ui.cb_simPeriod_2, SIGNAL(toggled(bool)), this, SLOT(setEctopicModS2(bool)));		
	QObject::connect(ui.b_snapShot, SIGNAL(clicked()), m_ioHandler, SLOT(stopCalculation()));
	QObject::connect(ui.b_snapShot, SIGNAL(clicked()), m_ioHandler, SLOT(setBmp()));
	QObject::connect(ui.b_snapShot, SIGNAL(clicked()), m_ioHandler, SLOT(saveAsBmp()));


	//ADDED
	QObject::connect(ui.b_saveStructure, SIGNAL(clicked()), m_ioHandler, SLOT(saveCurrentStructure()));
	QObject::connect(ui.b_loadStructure, SIGNAL(clicked()), m_ioHandler, SLOT(loadCustomStructure()));
	QObject::connect(ui.b_saveCurrentState, SIGNAL(clicked()), m_ioHandler, SLOT(saveCurrentState()));
	QObject::connect(ui.b_loadState, SIGNAL(clicked()), m_ioHandler, SLOT(loadCurrentState()));
	QObject::connect(ui.b_loadState, SIGNAL(clicked()), m_ioHandler, SLOT(loadCurrentState()));
	QObject::connect(ui.rb_setDisplayCsdMode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayCSD(bool)));
	QObject::connect(ui.rb_setDisplayPotentialMode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayPotential(bool)));
	QObject::connect(ui.rb_setDisplayC1Mode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayCurrent1(bool)));
	QObject::connect(ui.rb_setDisplayC2Mode, SIGNAL(toggled(bool)), glGraph, SLOT(setDisplayCurrent2(bool)));
	QObject::connect(ui.b_stateStructureModifier, SIGNAL(toggled(bool)), glGraph, SLOT(setStateStructureModifier(bool)));
	QObject::connect(ui.b_stateViewer, SIGNAL(toggled(bool)), glGraph, SLOT(setStateViewer(bool)));
	//ADDED END

	QObject::connect(ui.cb_calcEnt, SIGNAL(toggled(bool)), this, SLOT(setEntropyToggle(bool)));
	QObject::connect(ui.cb_calcPat, SIGNAL(toggled(bool)), this, SLOT(setEntropyPat(bool)));
	QObject::connect(ui.cb_calcVar, SIGNAL(toggled(bool)), this, SLOT(setEntropyVar(bool)));

//---------------------INDICATOR CONNECTIONS ------------------------------------------
	QObject::connect(Machine2d, SIGNAL(emitGlobalTimeOnly(double)), ui.lcd_time, SLOT(display(double)));

	QObject::connect(ui.b_setDiffusion, SIGNAL(clicked()), this, SLOT(stopCalculation()));
	QObject::connect(ui.b_setDiffusion, SIGNAL(clicked()), this, SLOT(setAtrialDiffusion()));
	QObject::connect(ui.b_saveDiffusion, SIGNAL(clicked()), m_ioHandler, SLOT(saveDiffusionToFile()));
	QObject::connect(ui.b_loadDiffusion, SIGNAL(clicked()), m_ioHandler, SLOT(readDiffusionFromFile()));
	QObject::connect(ui.b_clearDiffusion, SIGNAL(clicked()), this, SLOT(resetDiffusion()));

	QObject::connect(ui.rb_paintDiffusion, SIGNAL(toggled(bool)), diffusionPainter, SLOT(setCurrentPainter_Diffusion()));
	QObject::connect(ui.rb_paintAnisotrophy, SIGNAL(toggled(bool)), diffusionPainter, SLOT(setCurrentPainter_Anisotrophy()));
	
	QObject::connect(ui.dial_diffSizeN, SIGNAL(valueChanged(int)), diffusionPainter, SLOT(setSigma(int)));
	QObject::connect(ui.dial_diffValueN, SIGNAL(valueChanged(int)), diffusionPainter, SLOT(setAmplitude(int)));
	QObject::connect(ui.m_refSlider, SIGNAL(valueChanged(int)), this, SLOT(setAtrialRestitution(int)));


	QObject::connect(ui.actionO_programie, SIGNAL(activated()), msgBoxAbout, SLOT(exec()));
	QObject::connect(ui.actionInstrukcja, SIGNAL(activated()), this, SLOT(openInstruction()));

		
	QObject::connect(ui.actionSettings, SIGNAL(activated()), this, SLOT(openSettings()));
	QObject::connect(ui.actionSet_Default_Settings, SIGNAL(activated()), this, SLOT(stopCalculation()));
	QObject::connect(ui.actionSet_Default_Settings, SIGNAL(activated()), m_ioHandler, SLOT(getDefaultParameters()));

	QObject::connect(this->ui.b_setStructure, SIGNAL(clicked()), this, SLOT(stopCalculation()));
	QObject::connect(this->ui.b_setStructure, SIGNAL(clicked()), this, SLOT(setAtrialStructure()));
	
	QObject::connect(this->ui.b_stimPoint, SIGNAL(clicked()), this, SLOT(setEctopicPoint()));
	QObject::connect(this->ui.b_stimHorizontal, SIGNAL(clicked()), this, SLOT(setEctopicHoriz()));
	QObject::connect(this->ui.b_stimVert, SIGNAL(clicked()), this, SLOT(setEctopicVert()));


//---------------------GRAPH CONNECTIONS -------------------------------------------
	QObject::connect(diffusionPainter, SIGNAL(positionElektrode(int, int,int)), this, SLOT(setProbeElectrode(int,int,int)));
	QObject::connect( Machine2d->probeOscillator[0], SIGNAL(newPotentialTime(double,double)),plotPotentialE1, SLOT(addValue_e1(double,double)));
	QObject::connect( Machine2d->probeOscillator[1], SIGNAL(newPotentialTime(double, double)),plotPotentialE2, SLOT(addValue_e2(double,double)));
	QObject::connect( Machine2d->probeOscillator[2], SIGNAL(newPotentialTime(double, double)),plotPotentialE3, SLOT(addValue_e3(double,double)));
	QObject::connect(ui.rb_probe1, SIGNAL(toggled(bool)), this, SLOT(switchElectrode()));
	QObject::connect(ui.rb_probe2, SIGNAL(toggled(bool)), this, SLOT(switchElectrode()));
	QObject::connect(ui.rb_probe3, SIGNAL(toggled(bool)), this, SLOT(switchElectrode()));	

	QObject::connect( Machine2d->RRcalc_1, SIGNAL(nextRR(double)),plotRR_el1, SLOT(appendRR(double)));	
	QObject::connect( Machine2d->RRcalc_2, SIGNAL(nextRR(double)),plotRR_el2, SLOT(appendRR(double)));	
	QObject::connect( Machine2d->RRcalc_3, SIGNAL(nextRR(double)),plotRR_el3, SLOT(appendRR(double)));	


	QObject::connect(ui.b_potentialplot_clear, SIGNAL(clicked()), this, SLOT(plotPotentialClear()));
	QObject::connect(ui.b_potentialplot_autoscale, SIGNAL(clicked()), plotPotentialE1, SLOT(autoscale()));
	QObject::connect(ui.b_potentialplot_zoominY, SIGNAL(clicked()), plotPotentialE1, SLOT(zoomInY()));
	QObject::connect(ui.b_potentialplot_zoomoutY, SIGNAL(clicked()), plotPotentialE1, SLOT(zoomOutY()));
	QObject::connect(ui.b_potentialplot_zoominX, SIGNAL(clicked()), plotPotentialE1, SLOT(zoomInX()));
	QObject::connect(ui.b_potentialplot_zoomoutX, SIGNAL(clicked()), plotPotentialE1, SLOT(zoomOutX()));

	QObject::connect(ui.b_potentialplot_autoscale, SIGNAL(clicked()), plotPotentialE2, SLOT(autoscale()));
	QObject::connect(ui.b_potentialplot_zoominY, SIGNAL(clicked()), plotPotentialE2, SLOT(zoomInY()));
	QObject::connect(ui.b_potentialplot_zoomoutY, SIGNAL(clicked()), plotPotentialE2, SLOT(zoomOutY()));
	QObject::connect(ui.b_potentialplot_zoominX, SIGNAL(clicked()), plotPotentialE2, SLOT(zoomInX()));
	QObject::connect(ui.b_potentialplot_zoomoutX, SIGNAL(clicked()), plotPotentialE2, SLOT(zoomOutX()));

	QObject::connect(ui.b_potentialplot_autoscale, SIGNAL(clicked()), plotPotentialE3, SLOT(autoscale()));
	QObject::connect(ui.b_potentialplot_zoominY, SIGNAL(clicked()), plotPotentialE3, SLOT(zoomInY()));
	QObject::connect(ui.b_potentialplot_zoomoutY, SIGNAL(clicked()), plotPotentialE3, SLOT(zoomOutY()));
	QObject::connect(ui.b_potentialplot_zoominX, SIGNAL(clicked()), plotPotentialE3, SLOT(zoomInX()));
	QObject::connect(ui.b_potentialplot_zoomoutX, SIGNAL(clicked()), plotPotentialE3, SLOT(zoomOutX()));

	QObject::connect(ui.b_rrplot_autoscale, SIGNAL(clicked()), plotRR_el1, SLOT(autoscale()));
	QObject::connect(ui.b_rrplot_zoominX, SIGNAL(clicked()), plotRR_el1, SLOT(zoomInX()));
	QObject::connect(ui.b_rrplot_zoomoutX, SIGNAL(clicked()), plotRR_el1, SLOT(zoomOutX()));
	//QObject::connect(ui.b_rrplot_zoominY, SIGNAL(clicked()), plotRR_el1, SLOT(zoomInY()));
	//QObject::connect(ui.b_rrplot_zoomoutY, SIGNAL(clicked()), plotRR_el1, SLOT(zoomOutY()));
	QObject::connect(ui.sb_yminRR, SIGNAL(valueChanged(double)), plotRR_el1, SLOT(setYmin(double)));
	QObject::connect(ui.sb_ymaxRR, SIGNAL(valueChanged(double)), plotRR_el1, SLOT(setYmax(double)));

	QObject::connect(ui.b_rrplot_clear, SIGNAL(clicked()), this, SLOT(plotRRClear()));
	QObject::connect(ui.b_rrlplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(saveRRPlot_1()));
	QObject::connect(ui.b_rrlplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(saveRRPlot_2()));
	QObject::connect(ui.b_rrlplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(saveRRPlot_3()));

	QObject::connect(ui.b_rrplot_autoscale, SIGNAL(clicked()), plotRR_el2, SLOT(autoscale()));
	QObject::connect(ui.b_rrplot_zoominX, SIGNAL(clicked()), plotRR_el2, SLOT(zoomInX()));
	QObject::connect(ui.b_rrplot_zoomoutX, SIGNAL(clicked()), plotRR_el2, SLOT(zoomOutX()));
	QObject::connect(ui.sb_yminRR, SIGNAL(valueChanged(double)), plotRR_el2, SLOT(setYmin(double)));
	QObject::connect(ui.sb_ymaxRR, SIGNAL(valueChanged(double)), plotRR_el2, SLOT(setYmax(double)));

	QObject::connect(ui.b_rrplot_autoscale, SIGNAL(clicked()), plotRR_el3, SLOT(autoscale()));
	QObject::connect(ui.b_rrplot_zoominX, SIGNAL(clicked()), plotRR_el3, SLOT(zoomInX()));
	QObject::connect(ui.b_rrplot_zoomoutX, SIGNAL(clicked()), plotRR_el3, SLOT(zoomOutX()));
	QObject::connect(ui.sb_yminRR, SIGNAL(valueChanged(double)), plotRR_el3, SLOT(setYmin(double)));
	QObject::connect(ui.sb_ymaxRR, SIGNAL(valueChanged(double)), plotRR_el3, SLOT(setYmax(double)));



	QObject::connect(ui.b_rrplot_autoscale_3, SIGNAL(clicked()), plotEnt_el1, SLOT(autoscale()));
	QObject::connect(ui.b_rrplot_zoominX_3, SIGNAL(clicked()), plotEnt_el1, SLOT(zoomInX()));
	QObject::connect(ui.b_rrplot_zoomoutX_3, SIGNAL(clicked()), plotEnt_el1, SLOT(zoomOutX()));
	QObject::connect(ui.sb_yminEnt, SIGNAL(valueChanged(double)), plotEnt_el1, SLOT(setYmin(double)));
	QObject::connect(ui.sb_ymaxEnt, SIGNAL(valueChanged(double)), plotEnt_el1, SLOT(setYmax(double)));

	QObject::connect(ui.b_rrplot_autoscale_3, SIGNAL(clicked()), plotEnt_el2, SLOT(autoscale()));
	QObject::connect(ui.b_rrplot_zoominX_3, SIGNAL(clicked()), plotEnt_el2, SLOT(zoomInX()));
	QObject::connect(ui.b_rrplot_zoomoutX_3, SIGNAL(clicked()), plotEnt_el2, SLOT(zoomOutX()));
	QObject::connect(ui.sb_yminEnt, SIGNAL(valueChanged(double)), plotEnt_el2, SLOT(setYmin(double)));
	QObject::connect(ui.sb_ymaxEnt, SIGNAL(valueChanged(double)), plotEnt_el2, SLOT(setYmax(double)));

	QObject::connect(ui.b_rrplot_autoscale_3, SIGNAL(clicked()), plotEnt_el3, SLOT(autoscale()));
	QObject::connect(ui.b_rrplot_zoominX_3, SIGNAL(clicked()), plotEnt_el3, SLOT(zoomInX()));
	QObject::connect(ui.b_rrplot_zoomoutX_3, SIGNAL(clicked()), plotEnt_el3, SLOT(zoomOutX()));
	QObject::connect(ui.sb_yminEnt, SIGNAL(valueChanged(double)), plotEnt_el3, SLOT(setYmin(double)));
	QObject::connect(ui.sb_ymaxEnt, SIGNAL(valueChanged(double)), plotEnt_el3, SLOT(setYmax(double)));
	//QObject::connect(ui.b_rrplot_clear_3, SIGNAL(clicked()), this, SLOT(plotEntClear()));

	QObject::connect(ui.b_potentialplot_save, SIGNAL(clicked()), m_ioHandler, SLOT(savePotentialPlot()));

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
		plotPotentialE1->clear();
		plotPotentialE2->clear();
		plotPotentialE3->clear();
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
		plotRR_el1->clear();
	plotRR_el1->d_curve.front()->setPen(QPen(QBrush(QColor(255,255,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
		plotRR_el2->clear();
	plotRR_el2->d_curve.front()->setPen(QPen(QBrush(QColor(255,0,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
		plotRR_el3->clear();
	plotRR_el3->d_curve.front()->setPen(QPen(QBrush(QColor(0,255,0),Qt::SolidPattern), 2.0, Qt::SolidLine));
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
	if(ui.cb_saveBMP->isChecked())
	{
		m_ioHandler->saveAsBmp();
	}
	
	emit tajmer();
	glGraph->updateGL();

	static int iter = 1;
	static int iter2 = 1;
	
	///TODO: zrobiæ coœ jak jest last

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

		diffusionPainter->m_pixelmap->resetDefault(255,255,255);
		diffusionPainter->m_anisotrophy->resetDefault(150,150,0);
		diffusionPainter->updateGL();
		setAtrialDiffusion();
		///TODO tu coœ z elektrodami
	}
}
void SimpleHeart::setAtrialDiffusion()
{
	for (unsigned int j = 0; j < m_grid->getSize(); ++j)
	{
		for (unsigned int i = 0; i < m_grid->getSize(); ++i)
		{
			Machine2d->m_diffusionCoefficients[j][i] = diffusionPainter->m_pixelmap->m_matrix[j][i]->color.greenF()*(diffusionPainter->m_upperLimit - diffusionPainter->m_lowerLimit) + diffusionPainter->m_lowerLimit;
			Machine2d->m_anisotrophyCoefficients[j][i] = 0.5;// diffusionPainter->m_anisotrophy->m_matrix[j][i]->color.redF();
		
	}
	}
	Machine2d->editDiffusionCoefficients();
	ui.statusBar->showMessage("Diffusion set");


}

void SimpleHeart::setEntropyToggle(bool v)
{

	ui.statusBar->showMessage(tr("setEntropyToggle"));
	if(v)
	{
		ui.statusBar->showMessage("Entropy measurment ON");
		if(m_patternEnt)
		{
			QObject::connect( Machine2d->RRcalc_1, SIGNAL(nextPatEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
			QObject::connect( Machine2d->RRcalc_2, SIGNAL(nextPatEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
			QObject::connect( Machine2d->RRcalc_3, SIGNAL(nextPatEn(double)),plotEnt_el3, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextVarEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextVarEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextVarEn(double)),plotEnt_el3, SLOT(appendRR(double)));	

			
			QObject::connect(Machine2d->RRcalc_1, SIGNAL(nextRR(double)), Machine2d->RRcalc_1, SLOT(calculatePatEn(double)));
			QObject::connect(Machine2d->RRcalc_2, SIGNAL(nextRR(double)), Machine2d->RRcalc_2, SLOT(calculatePatEn(double)));
			QObject::connect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculatePatEn(double)));
			QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR(double)), Machine2d->RRcalc_2, SLOT(calculateVarEn(double)));
			QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
			QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
		}
		if(m_variabilityEnt)
		{
			QObject::connect( Machine2d->RRcalc_1, SIGNAL(nextVarEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
			QObject::connect( Machine2d->RRcalc_2, SIGNAL(nextVarEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
			QObject::connect( Machine2d->RRcalc_3, SIGNAL(nextVarEn(double)),plotEnt_el3, SLOT(appendRR(double)));
			QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextPatEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextPatEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextPatEn(double)),plotEnt_el3, SLOT(appendRR(double)));	

	
			QObject::disconnect(Machine2d->RRcalc_1, SIGNAL(nextRR()), Machine2d->RRcalc_1, SLOT(calculatePatEn()));
			QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR()), Machine2d->RRcalc_2, SLOT(calculatePatEn()));
			QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR()), Machine2d->RRcalc_3, SLOT(calculatePatEn()));
			QObject::connect(Machine2d->RRcalc_1, SIGNAL(nextRR()), Machine2d->RRcalc_1, SLOT(calculateVarEn()));
			QObject::connect(Machine2d->RRcalc_2, SIGNAL(nextRR()), Machine2d->RRcalc_2, SLOT(calculateVarEn()));
			QObject::connect(Machine2d->RRcalc_3, SIGNAL(nextRR()), Machine2d->RRcalc_3, SLOT(calculateVarEn()));
	
		}
	}
	else
	{
		ui.statusBar->showMessage("Entropy measurment OFF");
			QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextPatEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextPatEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextPatEn(double)),plotEnt_el3, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_1, SIGNAL(nextVarEn(double)),plotEnt_el1, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_2, SIGNAL(nextVarEn(double)),plotEnt_el2, SLOT(appendRR(double)));	
			QObject::disconnect( Machine2d->RRcalc_3, SIGNAL(nextVarEn(double)),plotEnt_el3, SLOT(appendRR(double)));	
			QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR(double)), Machine2d->RRcalc_2, SLOT(calculateVarEn(double)));
			QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
			QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR(double)), Machine2d->RRcalc_3, SLOT(calculateVarEn(double)));
			//QObject::disconnect(Machine2d->RRcalc_1, SIGNAL(nextRR()), Machine2d->RRcalc_1, SLOT(calculatePatEn()));
			//QObject::disconnect(Machine2d->RRcalc_2, SIGNAL(nextRR()), Machine2d->RRcalc_2, SLOT(calculatePatEn()));
			//QObject::disconnect(Machine2d->RRcalc_3, SIGNAL(nextRR()), Machine2d->RRcalc_3, SLOT(calculatePatEn()));
	}
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
	ui.m_refraction->setValue(value);
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
//		stopCalculation();
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

//--------------------------------------------------------------
void SimpleHeart::switchElectrode()
{
	if(ui.rb_probe1->isChecked())
	{
		diffusionPainter->setCurrentPainter_Selector();
		diffusionPainter->currentMoover=0;
	}
	else if(ui.rb_probe2->isChecked())
	{
		diffusionPainter->setCurrentPainter_Selector();
		diffusionPainter->currentMoover=1;
	}
	else if(ui.rb_probe3->isChecked())
	{
		diffusionPainter->setCurrentPainter_Selector();
		diffusionPainter->currentMoover=2;
	}
	else
	{
		diffusionPainter->setCurrentPainter_Diffusion();
	}
}
void SimpleHeart::setProbeElectrode(int electrode,int id_y ,int id_x)
{
	if(electrode == 0 ) 	QObject::disconnect( Machine2d->probeOscillator[0], SIGNAL(newPotentialTime(double,double)),plotPotentialE1, SLOT(addValue_e1(double,double)));
	else if(electrode == 1 ) 	QObject::disconnect( Machine2d->probeOscillator[1], SIGNAL(newPotentialTime(double,double)),plotPotentialE2, SLOT(addValue_e2(double,double)));
	else if(electrode == 2) 	QObject::disconnect( Machine2d->probeOscillator[2], SIGNAL(newPotentialTime(double,double)),plotPotentialE3, SLOT(addValue_e3(double,double)));

	//int ii = static_cast<double> (j % gridWidth);
	//int jj = floor(static_cast<double>(j) / static_cast<double>(gridWidth));
	//Machine2d->probeOscillator[electrode] = Machine2d->m_oscillators[id_y][id_x];

	Machine2d->probeOscillator[electrode] = m_grid->m_mesh[id_y + m_grid->getSize()*id_x];
		//Machine2d->m_oscillators[id_y][id_x];

	if(electrode == 0 ) QObject::connect( Machine2d->probeOscillator[0], SIGNAL(newPotentialTime(double,double)),plotPotentialE1, SLOT(addValue_e1(double,double)));
	else if(electrode == 1 ) QObject::connect( Machine2d->probeOscillator[1], SIGNAL(newPotentialTime(double,double)),plotPotentialE2, SLOT(addValue_e2(double,double)));
	else if(electrode == 2) QObject::connect( Machine2d->probeOscillator[2], SIGNAL(newPotentialTime(double,double)),plotPotentialE3, SLOT(addValue_e3(double,double)));

	glGraph->updateGL();
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