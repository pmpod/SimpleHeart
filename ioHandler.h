#pragma once
#include <QObject>
#include <QMessageBox>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>

#include "AtrialMachine2d.h"
#include "heartDefines.h"
#include "Model\cartesianGrid.h"
#include "DiffusionPainter.h"
#include "GLfunc.h"
#include <matio.h>

class SimpleHeart;

class ioHandler: public QObject
{
	Q_OBJECT

public:
	ioHandler(SimpleHeart* handle);
	~ioHandler(void);

public slots:


mat_t * initMatlabLoad(const char *inname);
bool saveCurrentStructure();
bool loadCurrentState();
CardiacMesh* loadCustomStructure();
bool saveCurrentState();




	void saveAsBmp();
	void setBmp();
	void saveStructureToFile();
	void readStructureFromFile();
	void readStructureFromFile(int type);
	void saveDiffusionToFile();
	void readDiffusionFromFile();
	void writeParametersToFile();
	void getParametersFromFile();
	void getDefaultParameters();
	void savePotentialPlot();
	void saveRRPlot_1();
	void saveRRPlot_2();
	void saveRRPlot_3();
public:
	template <class Ttype> matvar_t* readMatVariable(const char *varname, mat_t *matfp, Ttype* &outvar);

	SimpleHeart* m_handle;

	string intToStr(int n);
	int strToInt(string s);


	string paramFilename;

	string filePrefix();
	string m_prefix;

    QString pathParameters;
	ifstream inputParameters;
	ofstream backupParameters;
};

