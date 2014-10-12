#include "atrialParameters.h"

atrialParameters::atrialParameters(void)
{
	m_mainTimestep = 0.25;
	m_mainSkip = 3;
	m_algorithmType = SW_EULER;


	m_ectopicActivity = false;
	m_ectopicS1_toggle = false;
	m_ectopicS2_toggle = false;
	m_ectopicAmplitude = 1.0;
	m_ectopicPeriodS1 = 200;
	m_ectopicPeriodS2 = 75;
	m_ectopicLength = 50;
	 m_ectopicSizeX = 3;
	 m_ectopicSizeY = 3;
	 m_ectopicX = 50;
	 m_ectopicY = 50;
	 m_ectopicShape = 1;

	 m_saAlpha_1 = 0.1;
	 m_saF_1 = 0.00005;
	 m_saE_1 = 4.3;
 	 m_saD_1 = 3.0;
	 m_saV1_1 = 1.0;
	 m_saV2_1 = -1.0;
	 m_saTimeScaler_1 = 1.0;

	 m_saAlpha_2 = 0.1;
	 m_saF_2 = 0.00005;
	 m_saE_2 = 4.3;
 	 m_saD_2 = 3.0;
	 m_saV1_2 = 1.0;
	 m_saV2_2 = -1.0;
	 m_saTimeScaler_2 = 1.0;

	 m_avAlpha_1 = 0.1;
	 m_avF_1 = 0.00005;
	 m_avE_1 = 3.3;
 	 m_avD_1 = 3.0;
	 m_avV1_1 = 1.0;
	 m_avV2_1 = -1.0;
	 m_avTimeScaler_1 = 1.0;

	 m_avAlpha_2 = 0.1;
	 m_avF_2 = 0.00005;
	 m_avE_2 = 3.3;
 	 m_avD_2 = 3.0;
	 m_avV1_2 = 1.0;
	 m_avV2_2 = -1.0;
	 m_avTimeScaler_2 = 1.0;

	 m_atriumBeta_1 = 1.1;//0.7;
	 m_atriumNi_1 = 0.009;//0.01;
	 m_atriumGamma_1 = 0.4;//0.5;
	 m_atriumC1_1 = 0.16;//2.2;0.16
	 m_atriumC2_1 = 0.99;//2.0;
	 m_atriumAlpha_1 = 0.4;//0.05;
	 m_atriumTimeScaler_1 = 1.0;

	 m_atriumBeta_2 = 1.1;//0.7;
	 m_atriumNi_2 = 0.009;//0.01;
	 m_atriumGamma_2 = 0.4;//0.5;
	 m_atriumC1_2 = 0.16;//2.2;
	 m_atriumC2_2 = 0.99;//2.0;
	 m_atriumAlpha_2 = 0.4;//0.05;
	 m_atriumTimeScaler_2 = 1.0;

	saveRR = true;
	savePotentialPlots = true;
	savePhasePlots = true;
	savePictures = false;
	picturesDelay = 10;

	electrode1_positionX = 5;
	electrode1_positionY = 5;

	 electrode2_positionX = 50;
	 electrode2_positionY = 50;

	 electrode3_positionX = 98;
	 electrode3_positionY = 98;
}

atrialParameters::~atrialParameters(void)
{
}


void atrialParameters::setEuler() { m_algorithmType = SW_EULER; }
void atrialParameters::setKutta() { m_algorithmType = SW_KUTTA; }
void atrialParameters::setDuffort() { m_algorithmType = SW_DUFORTFRENKL; }
void atrialParameters::setMainTimeStep(double timestep) { m_mainTimestep = timestep; }
void atrialParameters::setSkip(int skip) { 	m_mainSkip = skip; }
//-------------------------
void atrialParameters::setEctopicSize(int value){ m_ectopicSizeX = value;  m_ectopicSizeY = value; }
void atrialParameters::setEctopicAmplitude(double value){ m_ectopicAmplitude = value; }
void atrialParameters::setEctopicPeriod(double value){ m_ectopicPeriodS1 = value; }
void atrialParameters::setEctopicLength(double value){ m_ectopicLength = value; }
void atrialParameters::setEctopicPosX(int value){ m_ectopicX = value; }
void atrialParameters::setEctopicPosY(int value){ m_ectopicY = value; }
void atrialParameters::setEctopicShape(int value){ m_ectopicShape = value; }
void atrialParameters::toggleEctopicActivity(bool value){ m_ectopicActivity = value; }
void atrialParameters::toggleEctopicModActivity(bool value){ m_ectopicS1_toggle = value; }
void atrialParameters::setEctoModTime(double value){ m_ectoModTime = value; }
//--------------------------------
void atrialParameters::setSaGlobalAlpha(double value) {m_saAlpha_1 = value;}
void atrialParameters::setSaGlobalF(double value) {m_saF_1 = value;}
void atrialParameters::setSaGlobalE(double value) {m_saE_1 = value;}
void atrialParameters::setSaGlobalD(double value) {m_saD_1 = value;}
void atrialParameters::setSaGlobalV1(double value) {m_saV1_1 = value;}
void atrialParameters::setSaGlobalV2(double value) {m_saV2_1 = value;}
void atrialParameters::setSaGlobalTimeScaler(double value) {m_saTimeScaler_1 = value;}
//--------------------------------
void atrialParameters::setSaGlobalAlpha_2(double value) {m_saAlpha_2 = value;}
void atrialParameters::setSaGlobalF_2(double value) {m_saF_2 = value;}
void atrialParameters::setSaGlobalE_2(double value) {m_saE_2 = value;}
void atrialParameters::setSaGlobalD_2(double value) {m_saD_2 = value;}
void atrialParameters::setSaGlobalV1_2(double value) {m_saV1_2 = value;}
void atrialParameters::setSaGlobalV2_2(double value) {m_saV2_2 = value;}
void atrialParameters::setSaGlobalTimeScaler_2(double value) {m_saTimeScaler_2 = value;}
//--------------------------------
void atrialParameters::setAvGlobalAlpha(double value) {m_avAlpha_1 = value; }
void atrialParameters::setAvGlobalF(double value) {m_avF_1 = value; }
void atrialParameters::setAvGlobalE(double value) {m_avE_1 = value; }
void atrialParameters::setAvGlobalD(double value) {m_avD_1 = value; }
void atrialParameters::setAvGlobalV1(double value) {m_avV1_1 = value; }
void atrialParameters::setAvGlobalV2(double value) {m_avV2_1 = value;}
void atrialParameters::setAvGlobalTimeScaler(double value) {m_avTimeScaler_1 = value; }
//--------------------------------
void atrialParameters::setAvGlobalAlpha_2(double value) {m_avAlpha_2 = value; }
void atrialParameters::setAvGlobalF_2(double value) {m_avF_2 = value; }
void atrialParameters::setAvGlobalE_2(double value) {m_avE_2 = value; }
void atrialParameters::setAvGlobalD_2(double value) {m_avD_2 = value; }
void atrialParameters::setAvGlobalV1_2(double value) {m_avV1_2 = value; }
void atrialParameters::setAvGlobalV2_2(double value) {m_avV2_2 = value; }
void atrialParameters::setAvGlobalTimeScaler_2(double value) {m_avTimeScaler_2 = value; }
//-------------------------------------------------------------------
void atrialParameters::setAtriumGlobalGamma(double value) {m_atriumGamma_1 = value; }
void atrialParameters::setAtriumGlobalBeta(double value) {m_atriumBeta_1 = value; }
void atrialParameters::setAtriumGlobalNi(double value) {m_atriumNi_1 = value; }
void atrialParameters::setAtriumGlobalC1(double value) {m_atriumC1_1 = value; }
void atrialParameters::setAtriumGlobalC2(double value) {m_atriumC2_1 = value; }
void atrialParameters::setAtriumGlobalAlpha(double value) {m_atriumAlpha_1 = value; }
void atrialParameters::setAtriumGlobalTimeScaler(double value) {m_atriumTimeScaler_1 = value; }
//-------------------------------------------------------------------
void atrialParameters::setAtriumGlobalGamma_2(double value) {m_atriumGamma_2 = value; }
void atrialParameters::setAtriumGlobalBeta_2(double value) {m_atriumBeta_2 = value; }
void atrialParameters::setAtriumGlobalNi_2(double value) {m_atriumNi_2 = value; }
void atrialParameters::setAtriumGlobalC1_2(double value) {m_atriumC1_2 = value; }
void atrialParameters::setAtriumGlobalC2_2(double value) {m_atriumC2_2 = value; }
void atrialParameters::setAtriumGlobalAlpha_2(double value) {m_atriumAlpha_2 = value; }
void atrialParameters::setAtriumGlobalTimeScaler_2(double value) {m_atriumTimeScaler_2 = value; }
//----------------------------------

void atrialParameters::setSaveRR(bool value) {saveRR = value; }
void atrialParameters::setSsavePotentialPlots(bool value) {savePotentialPlots = value; }
void atrialParameters::setSavePhasePlots(bool value) {savePhasePlots = value; }
void atrialParameters::setSavePictures(bool value) {savePictures = value; }
void atrialParameters::setPicturesDelay(int value) {picturesDelay = value; }

void atrialParameters::setElectrode1_positionX(int value){ electrode1_positionX = value; }
void atrialParameters::setElectrode1_positionY(int value){electrode1_positionY = value; }
void atrialParameters::setElectrode2_positionX(int value){electrode2_positionX = value; }
void atrialParameters::setElectrode2_positionY(int value){electrode2_positionY = value; }
void atrialParameters::setElectrode3_positionX(int value){electrode3_positionX = value; }
void atrialParameters::setElectrode3_positionY(int value){electrode3_positionY = value; }
void atrialParameters::setBinSize(double value)
{
	m_binSize = value;
}
void atrialParameters::setWindowSize(int value)
{
	m_windowSize = value;
}
void atrialParameters::setTau(int value)
{
	m_tau = value;
}

