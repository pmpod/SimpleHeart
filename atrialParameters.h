#pragma once
#include "heartDefines.h"


class atrialParameters: public QObject
{
	Q_OBJECT

public:
	atrialParameters(void);
	~atrialParameters(void);

	double m_mainTimestep;
	int m_mainSkip;
	SW_ALGORITHMS m_algorithmType;
	
	double m_ectoModTime;

	bool m_ectopicS1_toggle;
	bool m_ectopicS2_toggle;
	bool m_ectopicActivity;
	double m_ectopicAmplitude;
	double m_ectopicPeriodS1;
	double m_ectopicPeriodS2;
	double m_ectopicLength;
	int m_ectopicSizeX;
	int m_ectopicSizeY;
	int m_ectopicX;
	int m_ectopicY;
	int m_ectopicShape;

	int m_tau;
	int m_windowSize;
	double m_binSize;

	double m_saAlpha_1;
	double m_saF_1;
	double m_saE_1;
	double m_saD_1;
	double m_saV1_1;
	double m_saV2_1;
	double m_saTimeScaler_1;

	double m_saAlpha_2;
	double m_saF_2;
	double m_saE_2;
	double m_saD_2;
	double m_saV1_2;
	double m_saV2_2;
	double m_saTimeScaler_2;

	double m_avAlpha_1;
	double m_avF_1;
	double m_avE_1;
	double m_avD_1;
	double m_avV1_1;
	double m_avV2_1;
	double m_avTimeScaler_1;

	double m_avAlpha_2;
	double m_avF_2;
	double m_avE_2;
	double m_avD_2;
	double m_avV1_2;
	double m_avV2_2;
	double m_avTimeScaler_2;

	double m_atriumBeta_1;
	double m_atriumNi_1;
	double m_atriumGamma_1;
	double m_atriumC1_1;
	double m_atriumC2_1;
	double m_atriumAlpha_1;
	double m_atriumTimeScaler_1;

	double m_atriumBeta_2;
	double m_atriumNi_2;
	double m_atriumGamma_2;
	double m_atriumC1_2;
	double m_atriumC2_2;
	double m_atriumAlpha_2;
	double m_atriumTimeScaler_2;
	
	double m_maxDiff;

//---------------------------------------------------
	//OPCJE ZAPISYWANIA - tu lub w bolku case'u
	//
	//
	bool saveRR;
	bool savePotentialPlots;
	bool savePhasePlots;
	bool savePictures;
	int picturesDelay;

	int electrode1_positionX;
	int electrode1_positionY;

	int electrode2_positionX;
	int electrode2_positionY;

	int electrode3_positionX;
	int electrode3_positionY;

public slots:
	void setMainTimeStep(double timestep);
	void setSkip(int skip);
	void setEuler();
	void setDuffort();
	void setKutta();


//---------------------------------------------------
    void setEctopicSize(int value);
    void setEctopicAmplitude(double value);
    void setEctopicPeriod(double value);
    void setEctopicLength(double value);
    void setEctopicPosX(int value);
    void setEctopicPosY(int value);
    void setEctopicShape(int value);
	void toggleEctopicActivity(bool value);
	void toggleEctopicModActivity(bool value);
	void setEctoModTime(double value);

//---------------------------------------------------
    void setSaGlobalAlpha(double value);
    void setSaGlobalF(double value);
    void setSaGlobalD(double value);
    void setSaGlobalE(double value);
    void setSaGlobalV1(double value);
    void setSaGlobalV2(double value);
    void setSaGlobalTimeScaler(double value);

    void setSaGlobalAlpha_2(double value);
    void setSaGlobalF_2(double value);
    void setSaGlobalD_2(double value);
    void setSaGlobalE_2(double value);
    void setSaGlobalV1_2(double value);
    void setSaGlobalV2_2(double value);
    void setSaGlobalTimeScaler_2(double value);

    void setAvGlobalAlpha(double value);
    void setAvGlobalF(double value);
    void setAvGlobalD(double value);
    void setAvGlobalE(double value);
    void setAvGlobalV1(double value);
    void setAvGlobalV2(double value);
    void setAvGlobalTimeScaler(double value);

    void setAtriumGlobalGamma(double value);
    void setAtriumGlobalBeta(double value);
    void setAtriumGlobalNi(double value);
    void setAtriumGlobalC1(double value);
    void setAtriumGlobalC2(double value);
    void setAtriumGlobalAlpha(double value);
    void setAtriumGlobalTimeScaler(double value);

	void setAvGlobalAlpha_2(double value);
    void setAvGlobalF_2(double value);
    void setAvGlobalD_2(double value);
    void setAvGlobalE_2(double value);
    void setAvGlobalV1_2(double value);
    void setAvGlobalV2_2(double value);
    void setAvGlobalTimeScaler_2(double value);

    void setAtriumGlobalGamma_2(double value);
    void setAtriumGlobalBeta_2(double value);
    void setAtriumGlobalNi_2(double value);
    void setAtriumGlobalC1_2(double value);
    void setAtriumGlobalC2_2(double value);
    void setAtriumGlobalAlpha_2(double value);
    void setAtriumGlobalTimeScaler_2(double value);


	void setSaveRR(bool value);
	void setSsavePotentialPlots(bool value);
	void setSavePhasePlots(bool value);
	void setSavePictures(bool value);
	void setPicturesDelay(int value);

	void setElectrode1_positionX(int value);
	void setElectrode1_positionY(int value);
	void setElectrode2_positionX(int value);
	void setElectrode2_positionY(int value);
	void setElectrode3_positionX(int value);
	void setElectrode3_positionY(int value);

	void setBinSize(double value);
	void setTau(int value);
	void setWindowSize(int value);

};
