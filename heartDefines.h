#pragma once
#include <cmath>
#include <QObject>
#include <string>

//fonts:
using namespace std;


//#define max(x,y) ( (x) < (y) ? (y) : (x) )
//#define min(x,y) ( (x) < (y) ? (x) : (y) )

//for prince - dormand
#define ATTEMPTS 12
#define MIN_SCALE_FACTOR 0.125
#define MAX_SCALE_FACTOR 4000.0


#define TIME_MULTIP 0.1

#define MAX_SIMULATION_TIME 100000000.0

//#define SA_SIZE 15
//#define AV_SIZE 15
//#define ATRIUM_SIZE 75
//#define TOTAL_SIZE (SA_SIZE+AV_SIZE+ATRIUM_SIZE)

//class heartDefines: public QObject
//{
//	Q_OBJECT
//
//public:
//	heartDefines();
//
//	public slots:
//	void setSaSize(int);
//	void setAvSize(int);
//	void setAtriumSize(int);
//	void setBranchAvSize(int);
//	void setBranchAtriumSize(int);
//	void setBranchStart(int);
//	void setBranchEnd(int);
//	void toggleBranching(bool);
//
//
//public:
//
//	///---------------
//	void validateBranchPosition();
//	int saNodeSize_1d;
//	int avNodeSize_1d;
//	int atriumSize_1d;
//	int totalSize_1d;
//
//	int branchStart;
//	int branchEnd;
//	int branchAtriumSize;
//	int branchAvSize;
//	int branchLength;
//
//	bool branchingToggle;
//
//	///---------------
//	
//
//};

enum HEART_FUNCTION
{
	VAN_DER_GRU_CURRENT,
	VAN_DER_GRU_POTENTIAL,
	FHN_CURRENT,
	FHN_POTENTIAL,
	RFHN_CURRENT,
	RFHN_POTENTIAL,
	OSC_CURRENT,
	OSC_CURRENT_2,
	OSC_POTENTIAL,
	V3_CURRENT,
	V3_CURRENT_2,
	V3_POTENTIAL,
	VAN_DER_GRU_CURRE_2,
};

enum H_BORDERNESS
{
	H_LEFT,
	H_RIGHT,
	H_INTERIOR
};


enum V_BORDERNESS
{
	V_TOP,
	V_BOTTOM,
	V_INTERIOR
};

enum CELL_TYPE
{
	SA_NODE,
	AV_NODE,
	ATRIAL_TISSUE,
	PURKINIE_BUNDLE,
	AV_NODE_BRANCH,
	ATRIAL_BRANCH,
	ATRIAL_V3,
	SOLID_WALL,
	NONE,
};

enum OSC_PARAMETER
{
	VDG_ALPHA,
	VDG_V1,
	VDG_V2,
	VDG_E,
	VDG_D,
	VDG_F,
	VDG_TS,
	FHN_BETA,
	FHN_NI,
	FHN_GAMMA,
	FHN_TS,
	FHN_C1,
	FHN_C2,
	FHN_ALPHA,
	V3_CRN,
	V3_modifiedLuoRudy,
	V3_GuineaPig,
	V3_BeelerReuter,
	V3_modifiedBeelerReuter



};

enum SW_ALGORITHMS
{
	SW_KUTTA,
	SW_EULER,
	SW_DUFORTFRENKL
};

