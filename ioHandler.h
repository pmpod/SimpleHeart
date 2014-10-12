#pragma once
#include <QObject>
#include <QMessageBox>
#include <string>
#include <fstream>
#include <ctime>
#include <sstream>

#include "AtrialMachine2d.h"
#include "heartDefines.h"
#include "cartesianGrid.h"
#include "DiffusionPainter.h"
#include "GLfunc.h"

using namespace std;

class SimpleHeart;

class ioHandler: public QObject
{
	Q_OBJECT

public:
	ioHandler(SimpleHeart* handle);
	~ioHandler(void);

public slots:
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

