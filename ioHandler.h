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
#include "View\GLfunc.h"
#include <matio.h>

class SimpleHeart;

class ioHandler: public QObject
{
	Q_OBJECT

public:
	ioHandler(SimpleHeart* handle);
	~ioHandler(void);

public slots:


    // \brief Savess current structure to .mat file
	bool saveCurrentStructure();
	bool loadCurrentState();
	CardiacMesh* loadCustomStructure();
	bool saveCurrentState();




	void saveAsBmp();
	void setBmp();

	void writeParametersToFile();
	void getParametersFromFile();
	void getDefaultParameters();

	void savePotentialPlot();
	void saveRRPlot_1();
	void saveRRPlot_2();
	void saveRRPlot_3();
public:
	template <class Ttype> matvar_t* readMatVariable(const char *varname, mat_t *matfp, Ttype* &outvar);
	mat_t * initMatlabLoad(const char *inname);

	SimpleHeart* m_handle;



	string paramFilename;

	string filePrefix();
	string m_prefix;

    QString pathParameters;
	ifstream inputParameters;
	ofstream backupParameters;

	string intToStr(int n);
	int strToInt(string s);
};

