#include "ioHandler.h"
#include "simpleheart.h"
#include <stdlib.h>

#define VARNAME_MODEL_DATA "mesh"
#define VARNAME_DESC_DATA "data_description"

template <class Ttype> matvar_t* ioHandler::readMatVariable(const char *varname, mat_t *matfp, Ttype* &outvar)
{
	//[3] Read matlab variable 
	matvar_t* matvar = Mat_VarRead(matfp, varname);
	if (NULL == matvar) {
		QMessageBox::information(m_handle, "Simple Heart", "Model data not found, or error reading MAT file"); return nullptr;
	}
	int meshSize = matvar->dims[0];
	outvar = (Ttype *)malloc(sizeof(Ttype) * matvar->dims[1] * matvar->dims[0]);
	outvar = static_cast<Ttype*>(matvar->data);

	return matvar;
}

ioHandler::ioHandler(SimpleHeart* handle)
{
	m_handle = handle;
	m_prefix = filePrefix();
	paramFilename = "parameters.inf";
	getParametersFromFile();
}
ioHandler::~ioHandler(void)
{
}



mat_t * ioHandler::initMatlabLoad(const char *inname)
{
	mat_t *matfp;
	matfp = Mat_Open(inname, MAT_ACC_RDONLY);
	if (NULL == matfp) {
		QMessageBox::information(m_handle, "Simple Heart", "Error opening MAT file"); return nullptr;
	}
	return matfp;
}

bool ioHandler::loadCurrentState()
{
	//[1] Open file dialog
	QString newPath = QFileDialog::getOpenFileName(m_handle, tr("Open model simulation state"), pathParameters, tr("MATLAB FILES (*.mat)"));
	if (newPath.isEmpty())
	{
		QMessageBox::information(m_handle, "Simple Heart",
			"Ooops! Unable to read current simulation state.\n"
			"The path and filename were not specified correctly..");
		return nullptr;
	}

	//[2] Prepare pathname
	pathParameters = newPath;
	QByteArray ba = pathParameters.replace(" ", "\x20").toLatin1();
	const char *inname = ba.data();

	matvar_t *matvar;
	mat_t *matfp = initMatlabLoad(inname);

	//[3] Read matlab variable oscillator_ID
	INT32 *oscillator_ID =nullptr;
	matvar = readMatVariable("oscillator_ID", matfp, oscillator_ID);
	int meshSize = matvar->dims[0];

	//[3] Read matlab variable potential
	double *potential;
	matvar = readMatVariable("potential", matfp, potential);

	//[3] Read matlab variable otherCurrents
	double *currVariables = nullptr;
	matvar = readMatVariable("currVariables", matfp, currVariables);
	int noOfCurrents = matvar->dims[1];


	Mat_Close(matfp);



	m_handle->stopCalculation();
	//[3] Fill data to 2d double array form
	Oscillator* osc;

	for (int i = 0; i < meshSize; ++i)
	{
		int oscID = oscillator_ID[i];
		osc = m_handle->m_grid->m_mesh[oscID];
		osc->setPotential(potential[oscID]);
		osc->setElectrogram(potential[oscID]);
		for (short ll = 0; ll < noOfCurrents; ++ll)
			osc->setCurrent(currVariables[meshSize * ll + oscID],ll);
	}



	m_handle->stopCalculation();
	m_handle->Machine2d->m_strategy->synchronise();
	return true;
}


bool ioHandler::saveCurrentState()
{
	//[0] Creat proper filename
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save current simulation state. The path should not contain Polish letters!"), pathParameters, tr("MATLAB FILES (*.mat)"));
	if (newPath.isEmpty())
	{
		QMessageBox::information(m_handle, "Simple Heart", "Unable to save currentsimulation state.\n"
														   "The path and filename were not specified correctly");
		return false;
	}
	pathParameters = newPath;
	QByteArray ba = pathParameters.replace(" ", "\x20").toLatin1();
	const char *outname = ba.data();

	//[1] Prepare matrix sizes
	const int meshSize = m_handle->m_grid->m_mesh.size();

	//[2] Prepare data arrays in form accepted by MAT
	double *position_xyz = (double *)malloc(sizeof(double) * 3 * meshSize);
	INT32 *oscillator_ID = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	double *potential = (double *)malloc(sizeof(double) * 1 * meshSize);
	//TODO Handling multiple cell types with different variables number
	double *variables = (double *)malloc(sizeof(double) *  m_handle->m_grid->m_mesh[0]->getNumberOfCurrents() * meshSize);
	double *electrogram = (double *)malloc(sizeof(double) * 1 * meshSize);
	double *csd = (double *)malloc(sizeof(double) * 1 * meshSize);
	double *time_ms = (double *)malloc(sizeof(double) * 1);
	char description[] = "One frame data pack";


	//[3] Fill data to 2d double array form
	int type;
	Oscillator* osc;

	time_ms[0] = m_handle->m_grid->m_simulationTime;
	for (int i = 0; i < meshSize; ++i) 
	{
		osc = m_handle->m_grid->m_mesh[i];
		position_xyz[meshSize * 0 + i] = osc->m_x;
		position_xyz[meshSize * 1 + i] = osc->m_y;
		position_xyz[meshSize * 2 + i] = osc->m_z;
		oscillator_ID[i] = osc->oscillatorID;

		for (short k = 0; k < osc->getNumberOfCurrents();++k)
			variables[meshSize * k + i] = osc->m_v_current[k];

		potential[i] = osc->getPotential();
		electrogram[i] = osc->m_v_electrogram;
		csd[i] = osc->getLastCurrentSource();
	}

	//[4] Setup the output variables
	mat_t *mat = Mat_CreateVer(outname, NULL, MAT_FT_DEFAULT);
	matvar_t *matvar;
	size_t dims[2];
	dims[0] = meshSize;

	//[5] Save to mat file
	if (mat)
	{
		//[5.2] Save the second variable matvalues
		dims[1] = 3;
		matvar = Mat_VarCreate("position_xyz", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, position_xyz, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		matvar = Mat_VarCreate("oscillator_ID", MAT_C_INT32, MAT_T_INT32, 2,
			dims, oscillator_ID, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		matvar = Mat_VarCreate("potential", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, potential, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = m_handle->m_grid->m_mesh[0]->getNumberOfCurrents(); //TODO multiple number of currents
		matvar = Mat_VarCreate("currVariables", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, variables, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		matvar = Mat_VarCreate("electrogram", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, electrogram, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		matvar = Mat_VarCreate("curerntSourceDensity", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, csd, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = 1;
		matvar = Mat_VarCreate("time_ms", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, time_ms, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);


		dims[1] = strlen(description);
		dims[0] = 1;
		matvar = Mat_VarCreate("description", MAT_C_CHAR, MAT_T_UINT8, 2,
			dims, description, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		//[5.3] Close output
		Mat_Close(mat);
	}
	else
	{
		return false;
	}
	free(position_xyz);
	free(potential);
	free(electrogram);
	free(csd);
	free(time_ms);
	free(oscillator_ID);

	return true;
}

bool ioHandler::saveCurrentStructure()
{
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save current model structure"), pathParameters, tr("MATLAB FILES (*.mat)"));

	if (newPath.isEmpty())
	{
		QMessageBox::information(m_handle, "Simple Heart", "Unable to save current model structure to file.\n"
														   "The path and filename were not specified correctly");
		return false;
	}

	pathParameters = newPath;
	QByteArray ba = pathParameters.toLatin1();
	const char *outname = ba.data();


	//[1] Prepare matrix sizes
	const int meshSize = m_handle->m_grid->m_mesh.size();
	
	//[2] Prepare data arrays in form accepted by MAT
	double *position_xyz = (double *)malloc(sizeof(double) * 3 * meshSize);
	INT32 *oscillator_ID = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	INT32 *oscillator_TYPE = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	INT32 *neighbours_ID = (INT32 *)malloc(sizeof(INT32) * 6 * meshSize);


	//[3] Fill data to 2d double array form
	int type;
	Oscillator* osc;
	for (int i = 0; i < meshSize; ++i) {
		osc = m_handle->m_grid->m_mesh[i];
		oscillator_ID[i] = static_cast<INT32>(osc->oscillatorID);
		position_xyz[meshSize * 0 + i] = osc->m_x;
		position_xyz[meshSize * 1 + i] = osc->m_y;
		position_xyz[meshSize * 2 + i] = osc->m_z;
		//sigma_xyz[meshSize * 0 + i] = osc->m_sigmaX;
		//sigma_xyz[meshSize * 1 + i] = osc->m_sigmaY;
		//sigma_xyz[meshSize * 2 + i] = osc->m_sigmaZ;
		type = osc->m_type;
		oscillator_TYPE[i] = static_cast<INT32> (type);
		for (int currentNeighbour = 0; currentNeighbour < osc->m_neighbours.size(); ++currentNeighbour)
		{
			neighbours_ID[meshSize * (currentNeighbour)+i] = static_cast<INT32> (osc->m_neighbours[currentNeighbour]->oscillatorID);
		}
		for (int fillToEnd = osc->m_neighbours.size(); fillToEnd < 6; ++fillToEnd)
		{
			neighbours_ID[meshSize * fillToEnd + i] = static_cast<INT32> (-1);
		}
	}

	//[4] Setup the output variables
	mat_t *mat = Mat_CreateVer(outname, NULL, MAT_FT_DEFAULT);
	matvar_t *matvar;
	size_t dims[2];
	dims[0] = meshSize;

	//[5] Save to mat file
	if (mat)
	{
		//[5.2] Save the second variable matvalues
		dims[1] = 3;
		matvar = Mat_VarCreate("position_xyz", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, position_xyz, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		matvar = Mat_VarCreate("oscillator_ID", MAT_C_INT32, MAT_T_INT32, 2,
			dims, oscillator_ID, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		matvar = Mat_VarCreate("oscillator_TYPE", MAT_C_INT32, MAT_T_INT32, 2,
			dims, oscillator_TYPE, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 6;
		matvar = Mat_VarCreate("neighbours_ID", MAT_C_INT32, MAT_T_INT32, 2,
			dims, neighbours_ID, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		//[5.3] Close output
		Mat_Close(mat);
	}
	else
	{
		return false;
	}
	free(position_xyz);
	free(oscillator_TYPE);
	free(oscillator_ID);
	free(neighbours_ID);

	return true;
}
CardiacMesh* ioHandler::loadCustomStructure()
{
	//[1] Open file dialog
	QString newPath = QFileDialog::getOpenFileName(m_handle, tr("Open mdel structure file"), pathParameters, tr("MATLAB FILES (*.mat)"));
	if (newPath.isEmpty())
	{
		QMessageBox::information(m_handle, "Simple Heart",
			"Unable to read structure of the model.\n"
			"The path was not specified");
		return nullptr;
	}

	//[2] Prepare pathname
	pathParameters = newPath;
	QByteArray ba = pathParameters.toLatin1();
	const char *inname = ba.data();


	//[3] Read matlab variable VARNAME_MODEL_DATA
	mat_t *matfp;
	matvar_t *matvar;
	matfp = Mat_Open(inname, MAT_ACC_RDONLY);
	if (NULL == matfp) {
		QMessageBox::information(m_handle, "Simple Heart", "Error opening MAT file");
		return nullptr;
	}


	//[3] Read matlab variable oscillator_ID
	matvar = Mat_VarRead(matfp, "oscillator_ID");
	if (NULL == matvar) {
		QMessageBox::information(m_handle, "Simple Heart", "Model data not found, or error reading MAT file"); return nullptr; 
	}
	int meshSize = matvar->dims[0];
	INT32 *oscillator_ID = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	oscillator_ID = static_cast<INT32*>(matvar->data);

	//[3] Read matlab variable position_xyz
	matvar = Mat_VarRead(matfp, "position_xyz");
	if (NULL == matvar) {
		QMessageBox::information(m_handle, "Simple Heart", "Model data not found, or error reading MAT file"); return nullptr;
	}
	double *position_xyz = (double *)malloc(sizeof(double) * 3 * meshSize);
	position_xyz = static_cast<double*>(matvar->data);

	//[3] Read matlab variable oscillator_TYPE
	matvar = Mat_VarRead(matfp, "oscillator_TYPE");
	if (NULL == matvar) {
		QMessageBox::information(m_handle, "Simple Heart", "Model data not found, or error reading MAT file"); return nullptr;
	}
	INT32 *oscillator_TYPE = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	oscillator_TYPE = static_cast<INT32*>(matvar->data);

	//[3] Read matlab variable oscillator_TYPE
	matvar = Mat_VarRead(matfp, "neighbours_ID");
	if (NULL == matvar) {
		QMessageBox::information(m_handle, "Simple Heart", "Model data not found, or error reading MAT file"); return nullptr;
	}
	INT32 *neighbours_ID = (INT32 *)malloc(sizeof(INT32) * 6 * meshSize);
	neighbours_ID = static_cast<INT32*>(matvar->data);


	Mat_Close(matfp);

	CardiacMesh *grid = new CardiacMesh();
	int totalSize = meshSize;


	Oscillator *node = NULL;
	for (int j = 0; j < totalSize; ++j)
	{
		int temptype = oscillator_TYPE[j];
		CELL_TYPE type = static_cast<CELL_TYPE> (temptype);
		if (type == ATRIAL_V3)
			node = new v3model();
		else if (type == SOLID_WALL)
			node = new Oscillator();
		else 
			node = new Oscillator();

		node->setPositionX(position_xyz[totalSize * 0 + j]);
		node->setPositionY(position_xyz[totalSize * 1 + j]);
		node->setPositionZ(position_xyz[totalSize * 2 + j]);
		node->setType(type);
		node->oscillatorID = oscillator_ID[j];
		grid->m_mesh.push_back(node);
	}

	//ADD NEIGHBOURS
	double deltaMin = DBL_MAX;
	double deltaMax = 0.0;
	double mindim = 0;
	double maxdim = 0;
	double mintemp = 0;
	double maxtemp = 0;
	for (int j = 0; j < totalSize; ++j)
	{
		int ng = 0;
		while (neighbours_ID[totalSize* ng + j] != -1)
		{
			grid->m_mesh[j]->addNeighbour(grid->m_mesh[neighbours_ID[totalSize* ng + j]]);
			++ng;
		}

		if (deltaMin > grid->m_mesh[j]->m_closestDistanceID)
			deltaMin = grid->m_mesh[j]->m_closestDistanceID;
		if (deltaMax < grid->m_mesh[j]->m_farthestDistanceID)
			deltaMax = grid->m_mesh[j]->m_farthestDistanceID;

		mintemp = min(grid->m_mesh[j]->m_x, grid->m_mesh[j]->m_y);
		mintemp = min(mintemp, grid->m_mesh[j]->m_z);
		if (mindim > mintemp)
		{
			mindim = mintemp;
		}
		maxtemp = max(grid->m_mesh[j]->m_x, grid->m_mesh[j]->m_y);
		maxtemp = max(maxtemp, grid->m_mesh[j]->m_z);
		if (maxdim < maxtemp)
		{
			maxdim = maxtemp;
		}
	}


	grid->m_minimumDistance = deltaMin;
	grid->m_maximumDistance = deltaMax;
	grid->m_size = maxdim / deltaMin;

	grid->setVertexTriangleList(false);
	grid->calculateCenter();
	grid->setWallCells();

	return grid;
}
void ioHandler::writeParametersToFile()
{
	
		ofstream outputParameters;
	 outputParameters.open(paramFilename.c_str());
		
		if(outputParameters.fail())
		{
			QMessageBox::information(m_handle, tr("Base"),
			"+++ ERROR!!! The file does not exist.... +++ ");
   		}
		else
		{
			outputParameters << "# Data input file for Heart Dynamics" << endl;
			outputParameters << "# Prepared:" <<m_prefix<< endl;
			outputParameters << "# Data input file for Heart Dynamics" << endl;
			outputParameters << "" << endl;
			
			outputParameters << "SIMULATION" << endl;
			outputParameters << "step \t"<< m_handle->Machine2d->m_definitions->m_mainTimestep<<endl;
			outputParameters << "skip \t"<< m_handle->Machine2d->m_definitions->m_mainSkip<<endl;
			outputParameters << "maxdiff \t"<< m_handle->diffusionPainter->m_upperLimit<<endl;
			outputParameters << "ectosizeX \t"<<  m_handle->Machine2d->m_definitions->m_ectopicSizeX<<endl;
			outputParameters << "ectosizeY \t"<<  m_handle->Machine2d->m_definitions->m_ectopicSizeY<<endl;
			outputParameters << "ectoamp \t"<< m_handle->Machine2d->m_definitions->m_ectopicAmplitude<<endl;
			outputParameters << "ectoperiodS1 \t"<<  m_handle->Machine2d->m_definitions->m_ectopicPeriodS1<<endl;
			outputParameters << "ectoperiodS2 \t"<<  m_handle->Machine2d->m_definitions->m_ectopicPeriodS2<<endl;
			outputParameters << "ectolength \t"<< m_handle->Machine2d->m_definitions->m_ectopicLength<<endl;
			outputParameters << "tau \t"<<  m_handle->Machine2d->m_definitions->m_tau<<endl;
			outputParameters << "winsize \t"<<  m_handle->Machine2d->m_definitions->m_windowSize<<endl;
			outputParameters << "binsize \t"<< m_handle->Machine2d->m_definitions->m_binSize<<endl;
			outputParameters << "!END" << endl;

			outputParameters << "" << endl;
			outputParameters << "SA_NODE" << endl;
			outputParameters << "alpha \t"<< m_handle->Machine2d->m_definitions->m_saAlpha_1<<endl;
			outputParameters << "v1 \t"<< m_handle->Machine2d->m_definitions->m_saV1_1<<endl;
			outputParameters << "v2 \t"<< m_handle->Machine2d->m_definitions->m_saV2_1<<endl;
			outputParameters << "e \t"<<m_handle->Machine2d->m_definitions->m_saE_1<<endl;
			outputParameters << "d \t"<< m_handle->Machine2d->m_definitions->m_saD_1<<endl;
			outputParameters << "f \t"<< m_handle->Machine2d->m_definitions->m_saF_1<<endl;
			outputParameters << "!END" << endl;

			outputParameters << "" << endl;
			outputParameters << "ATRIUM" << endl;
			outputParameters << "beta \t"<< m_handle->Machine2d->m_definitions->m_atriumBeta_1<<endl;
			outputParameters << "gamma \t"<< m_handle->Machine2d->m_definitions->m_atriumGamma_1<<endl;
			outputParameters << "ni \t"<< m_handle->Machine2d->m_definitions->m_atriumNi_1<<endl;
			outputParameters << "alpha \t"<< m_handle->Machine2d->m_definitions->m_atriumAlpha_1<<endl;
			outputParameters << "C1 \t"<< m_handle->Machine2d->m_definitions->m_atriumC1_1<<endl;
			outputParameters << "C2 \t"<< m_handle->Machine2d->m_definitions->m_atriumC2_1<<endl;
			outputParameters << "!END" << endl;

			outputParameters << "" << endl;
			outputParameters << "AV_NODE" << endl;
			outputParameters << "alpha \t"<< m_handle->Machine2d->m_definitions->m_avAlpha_1<<endl;
			outputParameters << "v1 \t"<< m_handle->Machine2d->m_definitions->m_avV1_1<<endl;
			outputParameters << "v2 \t"<< m_handle->Machine2d->m_definitions->m_avV2_1<<endl;
			outputParameters << "e \t"<<m_handle->Machine2d->m_definitions->m_avE_1<<endl;
			outputParameters << "d \t"<< m_handle->Machine2d->m_definitions->m_avD_1<<endl;
			outputParameters << "f \t"<< m_handle->Machine2d->m_definitions->m_avF_1<<endl;
			outputParameters << "!END" << endl;
			outputParameters << "" << endl;
			outputParameters.close();
		}
}
void ioHandler::getDefaultParameters()
{
	
	paramFilename = "parametersDefault.inf";
	getParametersFromFile();
	paramFilename = "parameters.inf";

}
//-----------------------
void ioHandler::getParametersFromFile()
{
	inputParameters.open(paramFilename.c_str());
    if(inputParameters.fail())
    {
		QMessageBox::information(m_handle, tr("Base"),
		 "+++ ERROR!!! The file does not exist.... +++ ");
   	}
    else
    {
        string strOneLine = "";
		string parameterName = "";
		double parameterValue = 0;

    	while (inputParameters)
    	{
       		std::getline( inputParameters, strOneLine, '\n' );
       		//skip the comment at the beggining
            while ( strOneLine.find("#") != string::npos )
            {
                std::getline( inputParameters, strOneLine, '\n' );
            }

            stringstream oss;
			//QMessageBox::information(this, tr("Base"),
			//				   tr(strOneLine.c_str()));
			oss.clear();
				if(strOneLine == "SIMULATION")
				{
					std::getline( inputParameters, strOneLine, '\n' );
					oss.clear();
					oss << strOneLine.c_str();
					while(strOneLine != "!END")
					{
						oss >> parameterName;
						oss >> parameterValue;

						if( parameterName == "step" ) 
						{
							m_handle->Machine2d->m_definitions->setMainTimeStep(parameterValue);
							//m_handle->Machine2d->setMainTimeStep(parameterValue);
						}
						else if( parameterName == "skip" ) 
						{
							m_handle->Machine2d->m_definitions->setSkip(parameterValue);
							m_handle->Machine2d->setSkip(parameterValue);
						}
						else if( parameterName == "skip" ) 
						{
							m_handle->Machine2d->m_definitions->setSkip(parameterValue);
							m_handle->Machine2d->setSkip(parameterValue);
						}
						else if( parameterName == "maxdiff" ) 
						{
							m_handle->diffusionPainter->m_upperLimit = parameterValue;
							m_handle->diffusionPainter->m_lowerLimit = 0;
							m_handle->Machine2d->m_definitions->m_maxDiff = parameterValue;
							m_handle->setAtrialDiffusion();
						}
						else if( parameterName == "ectosizeX" ) 
						{
							m_handle->Machine2d->m_definitions->m_ectopicSizeX=parameterValue;
						}
						else if( parameterName == "ectosizeY" ) 
						{
							m_handle->Machine2d->m_definitions->m_ectopicSizeY=parameterValue;
						}
						else if( parameterName == "ectoamp" ) 
						{
							m_handle->Machine2d->m_definitions->m_ectopicAmplitude=parameterValue;
						}
						else if( parameterName == "ectoperiodS1" ) 
						{
							m_handle->Machine2d->m_definitions->m_ectopicPeriodS1=parameterValue;
							m_handle->ui.sb_simPeriod->setValue(parameterValue);
						}
						else if( parameterName == "ectoperiodS2" ) 
						{
							m_handle->Machine2d->m_definitions->m_ectopicPeriodS2=parameterValue;
							m_handle->ui.sb_simPeriod_2->setValue(parameterValue);
						}
						else if( parameterName == "ectolength" ) 
						{
							m_handle->Machine2d->m_definitions->m_ectopicLength=parameterValue;
						}
						else if( parameterName == "tau" ) 
						{
							m_handle->Machine2d->m_definitions->m_tau=parameterValue;
							//m_handle->ui.sb_tau->setValue(parameterValue);
						}
						else if( parameterName == "binsize" ) 
						{
							m_handle->Machine2d->m_definitions->m_binSize=parameterValue;
							//m_handle->ui.sb_binSize->setValue(parameterValue);
						}
						else if( parameterName == "winsize" ) 
						{
							m_handle->Machine2d->m_definitions->m_windowSize=parameterValue;
							//m_handle->ui.sb_winSize->setValue(parameterValue);
						}
						std::getline( inputParameters, strOneLine, '\n' );
						oss.clear();
						oss << strOneLine;
					}
				}
				if(strOneLine == "SA_NODE")
				{
					std::getline( inputParameters, strOneLine, '\n' );
					oss.clear();
					oss << strOneLine.c_str();
					while(strOneLine != "!END")
					{
						oss >> parameterName;
						oss >> parameterValue;

						if( parameterName == "alpha" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalAlpha(parameterValue);
							//m_handle->Machine2d->setSaGlobalAlpha(parameterValue);
						}
						else if( parameterName == "v1" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalV1(parameterValue);
							//m_handle->Machine2d->setSaGlobalV1(parameterValue);
						}
						else if( parameterName == "v2" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalV2(parameterValue);
							//m_handle->Machine2d->setSaGlobalV2(parameterValue);
						}
						else if( parameterName == "d" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalD(parameterValue);
							//m_handle->Machine2d->setSaGlobalD(parameterValue);
						}
						else if( parameterName == "e" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalE(parameterValue);
							//m_handle->Machine2d->setSaGlobalE(parameterValue);
						}
						else if( parameterName == "f" )
						{
							m_handle->Machine2d->m_definitions->setSaGlobalF(parameterValue);
							//m_handle->Machine2d->setSaGlobalF(parameterValue);
						}						
						//	else if( parameterValue == "deltax" )
						//	else if( parameterValue == "deltax" )
						std::getline( inputParameters, strOneLine, '\n' );
						oss.clear();
						oss << strOneLine;
					}
				}
				if(strOneLine == "SA_NODE_2")
				{
					std::getline( inputParameters, strOneLine, '\n' );
					oss.clear();
					oss << strOneLine.c_str();
					while(strOneLine != "!END")
					{
						oss >> parameterName;
						oss >> parameterValue;

						if( parameterName == "alpha" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalAlpha_2(parameterValue);
							//m_handle->Machine2d->setSaGlobalAlpha_2(parameterValue);
						}
						else if( parameterName == "v1" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalV1_2(parameterValue);
							//m_handle->Machine2d->setSaGlobalV1_2(parameterValue);
						}
						else if( parameterName == "v2" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalV2_2(parameterValue);
							//m_handle->Machine2d->setSaGlobalV(parameterValue);
						}
						else if( parameterName == "d" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalD_2(parameterValue);
							//m_handle->Machine2d->setSaGlobalD_2(parameterValue);
						}
						else if( parameterName == "e" ) 
						{
							m_handle->Machine2d->m_definitions->setSaGlobalE_2(parameterValue);
							//m_handle->Machine2d->setSaGlobalE(parameterValue);
						}
						else if( parameterName == "f" )
						{
							m_handle->Machine2d->m_definitions->setSaGlobalF_2(parameterValue);
							//m_handle->Machine2d->setSaGlobalF(parameterValue);
						}						
						//	else if( parameterValue == "deltax" )
						//	else if( parameterValue == "deltax" )
						std::getline( inputParameters, strOneLine, '\n' );
						oss.clear();
						oss << strOneLine;
					}
				}
				else if(strOneLine == "ATRIUM")
				{
						std::getline( inputParameters, strOneLine, '\n' );
						oss.clear();
						oss << strOneLine;

					while(strOneLine != "!END")
					{
						oss >> parameterName;
						oss >> parameterValue;
						if( parameterName == "beta" ) 
						{
							m_handle->Machine2d->m_definitions->setAtriumGlobalBeta(parameterValue);
							//m_handle->Machine2d->setAtriumGlobalBeta(parameterValue);
						}
						else if( parameterName == "gamma" ) 
						{
							m_handle->Machine2d->m_definitions->setAtriumGlobalGamma(parameterValue);
						//	m_handle->Machine2d->setAtriumGlobalGamma(parameterValue);
						}
						else if( parameterName == "ni" ) 
						{
							m_handle->Machine2d->m_definitions->setAtriumGlobalNi(parameterValue);
							//m_handle->Machine2d->setAtriumGlobalNi(parameterValue);
							m_handle->ui.m_refSlider->setValue(static_cast<int>((100-parameterValue*1000)));
							m_handle->ui.m_refraction->setValue(static_cast<int>((100-parameterValue*1000)));

						}
						else if( parameterName == "alpha" ) 
						{
							m_handle->Machine2d->m_definitions->setAtriumGlobalAlpha(parameterValue);
						//	m_handle->Machine2d->setAtriumGlobalAlpha(parameterValue);
						}
						else if( parameterName == "C1" ) 
						{
							m_handle->Machine2d->m_definitions->setAtriumGlobalC1(parameterValue);
						//	m_handle->Machine2d->setAtriumGlobalC1(parameterValue);
						}
						else if( parameterName == "C2" ) 
						{
							m_handle->Machine2d->m_definitions->setAtriumGlobalC2(parameterValue);
						//	m_handle->Machine2d->setAtriumGlobalC2(parameterValue);
						}
						//	else if( parameterValue == "deltax" )
						//	else if( parameterValue == "deltax" )
						std::getline( inputParameters, strOneLine, '\n' );
						oss.clear();
						oss << strOneLine;
					}
				}
				else if(strOneLine == "AV_NODE")
				{
						std::getline( inputParameters, strOneLine, '\n' );
						oss << strOneLine;
					while(strOneLine != "!END")
					{
						oss >> parameterName;
						oss >> parameterValue;
						if( parameterName == "alpha" ) 
						{
							m_handle->Machine2d->m_definitions->setAvGlobalAlpha(parameterValue);
						//	m_handle->Machine2d->setAvGlobalAlpha(parameterValue);
						}
						else if( parameterName == "v1" ) 
						{
							m_handle->Machine2d->m_definitions->setAvGlobalV1(parameterValue);
						//	m_handle->Machine2d->setAvGlobalV1(parameterValue);
						}
						else if( parameterName == "v2" ) 
						{
							m_handle->Machine2d->m_definitions->setAvGlobalV2(parameterValue);
						//	m_handle->Machine2d->setAvGlobalV2(parameterValue);
						}
						else if( parameterName == "d" ) 
						{
							m_handle->Machine2d->m_definitions->setAvGlobalD(parameterValue);
						//	m_handle->Machine2d->setAvGlobalD(parameterValue);
						}
						else if( parameterName == "e" ) 
						{
							m_handle->Machine2d->m_definitions->setAvGlobalE(parameterValue);
						//	m_handle->Machine2d->setAvGlobalE(parameterValue);
						}
						else if( parameterName == "f" )
						{
							m_handle->Machine2d->m_definitions->setAvGlobalF(parameterValue);
						//	m_handle->Machine2d->setAvGlobalF(parameterValue);
						}						
						//	else if( parameterValue == "deltax" )
						//	else if( parameterValue == "deltax" )
						std::getline( inputParameters, strOneLine, '\n' );
						oss.clear();
						oss << strOneLine;
					}
				}
				else
				{
				}
			}
	}
	inputParameters.close();
}
//----------------------------------
//------------------------------------------------
void ioHandler::saveAsBmp()
{
	//QString newPath = pathParameters;
	//
	/////TODO - save log here also
	//QImage* qi = new QImage(m_handle->Machine2d->m_oscillators.size(),m_handle->Machine2d->m_oscillators[0].size(), QImage::Format_RGB32);

	//for( int i = 0; i < m_handle->Machine2d->m_oscillators.size(); i++)
	//{
	//	for(int j = 0; j <  m_handle->Machine2d->m_oscillators[i].size(); j++)
	//	{
	//		 double rrr;
	//		 double ggg;
	//		 double bbb;
	//		float kolor = 0.3 + (m_handle->Machine2d->m_oscillators[j][i]->getPotential())/2.0;
	//		HSVtoRGB(1-sqrt(kolor),1.0,0.2+0.8*kolor,&rrr,&ggg,&bbb);
	//		
	//		 qi->setPixel(i, j, qRgb(static_cast<int>(rrr*200),static_cast<int>(ggg*200),static_cast<int>(bbb*200)));
	//	}
	//}
	//stringstream version;
	//version << m_handle->Machine2d->m_globalTime;
	//	newPath.append(QString(version.str().c_str()));
	//newPath.append(".png");
	//m_handle->ui.statusBar->showMessage(newPath);	
	//qi->save(newPath, 0, 100 );
}

//------------------------------------------------
void ioHandler::setBmp()
{

		QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save visualization result"),
			pathParameters, tr("any file (*)"));

		if (newPath.isEmpty())
		{
			QMessageBox::information( m_handle, "SimpleHeart",
			"Unable to save BMP.\n"
			 "The path was not specified" );
			return;
		}
		pathParameters = newPath;


}
//-----------------------------------------------
void ioHandler::saveStructureToFile()
{
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save Structure"),
        pathParameters, tr("struc files (*.struc)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save structure of atrium.\n"
		 "The path was not specified" );
        return;
	}

    pathParameters = newPath;

	ofstream output;
	output.open(pathParameters.toStdString().c_str());
	if(output.fail())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save structure of atrium.\n"
		 "Output file corrupted" );
   	}
	else
	{
		output << "# Data input file for Heart Dynamics" << endl;
		output << "# Prepared:" <<m_prefix<< endl;
		output << "# Data input file for Heart Dynamics" << endl;
		output << "" << endl;

		output << "SIZEX \t" << m_handle->m_grid->getSize()<<endl;
		output << "SIZEY \t" << m_handle->m_grid->getSize()<<endl;
		for (int j = 0; j < m_handle->m_grid->getSize(); ++j)
		{
			for (int i = 0; i <  m_handle->m_grid->getSize(); ++i)
			{
				//output << m_handle->m_grid->m_net[j][i]->m_type  << " ";
			}
			output << endl;
		}
	}
	output.close();
	m_handle->ui.statusBar->showMessage("Structure saved");
}
////------------------------------------------------

void ioHandler::readStructureFromFile(int type)
{

//	m_handle->ui.statusBar->showMessage( intToStr(type).c_str());
	
	QString newPath = QDir::currentPath();
	//newPath.append("/wholeHeart.struc");
	if (type == 1)
	{	
		newPath.append("/atrium.struc");
	}
	else if (type == 0)
	{	
		newPath.append("/tissueStrip.struc");
	}
	else if (type == 2)
	{	
		newPath.append("/cylinder.struc");
	}
	else if (type == 3)
	{	
		newPath.append("/wholeHeart.struc");
	}
	else if (type == 4)
	{	
		newPath.append("/two_bars.struc");
	}
	else if (type == 5)
	{	
		newPath.append("/pacing_recording.struc");
	}
	else if (type == 6)
	{	
		newPath.append("/coronarysinus.struc");
	}
	else
	{
		newPath.append("/atrium.struc");
	}

    pathParameters = newPath;
	ifstream inputStructure;
	inputStructure.open(pathParameters.toStdString().c_str());

    if(inputStructure.fail())
    {
		
		m_handle->ui.statusBar->showMessage(pathParameters.toStdString().c_str());
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save structure of atrium.\n"
		 "Input file corrupted" );
        return;
   	}
    else
    {
		m_handle->stopCalculation();
        string strOneLine = "";
		string parameterName = "";
		int size_x;
		int size_y;
		int parameterInt = 0;
		//CELL_TYPE type;
  //  	if(inputStructure)
  //  	{
  //     		std::getline( inputStructure, strOneLine, '\n' );
  //     		//skip the comment at the beggining
  //          while ( strOneLine.find("#") != string::npos )
		//	{
  //              std::getline( inputStructure, strOneLine, '\n' );
		//	}
  //             std::getline( inputStructure, strOneLine, '\n' );
		//	std::stringstream oss;
		//	oss << strOneLine.c_str();	
		//	oss >> parameterName;
		//	oss >> parameterInt;
		//	if( parameterName == "SIZEX" ) 
		//	{
		//		size_x = parameterInt;
		//	}
		//	oss.clear();
  //          std::getline( inputStructure, strOneLine, '\n' );
		//	oss << strOneLine.c_str();	
		//	oss >> parameterName;
		//	oss >> parameterInt;
		//	if( parameterName == "SIZEY" ) 
		//	{
		//		size_y = parameterInt;
		//	}
		//	for(int j = 0; j < size_y;++j)
		//	{
		//	    std::getline( inputStructure, strOneLine, '\n' );
		//		oss.clear();
		//		oss << strOneLine.c_str();
		//		for(int i = 0; i < size_x;++i)
		//		{
		//			oss >> parameterInt;
		//			switch (parameterInt)
		//			{
		//				case 0:
		//					m_handle->m_grid->m_net[j][i]->m_type = SA_NODE;
		//					m_handle->m_grid->m_net[j][i]->m_color = QColor(255,255,0);
		//					break;
		//				case 2:
		//					//m_handle->m_grid->m_net[j][i]->m_type = ATRIAL_TISSUE;
		//					m_handle->m_grid->m_net[j][i]->m_type = ATRIAL_V3;
		//					m_handle->m_grid->m_net[j][i]->m_color = QColor(255,255,255);	
		//					break;
		//				case 1:
		//					m_handle->m_grid->m_net[j][i]->m_type = AV_NODE;
		//					m_handle->m_grid->m_net[j][i]->m_color = QColor(0,255,0);		
		//					break;
		//				case 6:
		//					m_handle->m_grid->m_net[j][i]->m_type = SOLID_WALL;
		//					m_handle->m_grid->m_net[j][i]->m_color = QColor(0,0,0);	
		//					break;							
		//				case 3:
		//					m_handle->m_grid->m_net[j][i]->m_type = PURKINIE_BUNDLE;
		//					m_handle->m_grid->m_net[j][i]->m_color = QColor(200,255,200);	
		//					break;
		//				case 4:
		//					m_handle->m_grid->m_net[j][i]->m_type = AV_NODE_BRANCH;
		//					m_handle->m_grid->m_net[j][i]->m_color = QColor(100,255,100);	
		//					break;
		//				case 5:
		//					m_handle->m_grid->m_net[j][i]->m_type = ATRIAL_BRANCH;
		//					m_handle->m_grid->m_net[j][i]->m_color = QColor(200,255,200);	
		//					break;
		//			}
		//		}
		//	}
		//}
		////m_handle->structurePainter->refreshPainter();
		////m_handle->structurePainter->updateGL();
		//m_handle->Machine2d->setAtrialStructure();
	}
	inputStructure.close();
	m_handle->ui.statusBar->showMessage("Ready");
	pathParameters = QDir::currentPath();
}
void ioHandler::readStructureFromFile( )
{
	if ( QMessageBox::question( m_handle, tr("Overwrite current structure?"), tr("Overwrite current structure?"), tr("&Yes"), tr("&No"),  QString::null, 0, 1 ) )
	{
		return;
	}

    QString newPath = QFileDialog::getOpenFileName(m_handle, tr("Open structure file"), pathParameters, tr("structure files (*.struc)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to read structure of atrium.\n"
		 "The path was not specified" );
        return;
	}

    pathParameters = newPath;
	ifstream inputStructure;
	inputStructure.open(pathParameters.toStdString().c_str());

    if(inputStructure.fail())
    {
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save structure of atrium.\n"
		 "Input file corrupted" );
        return;
   	}
    else
    {
		m_handle->stopCalculation();
        string strOneLine = "";
		string parameterName = "";
		int size_x;
		int size_y;
		int parameterInt = 0;
		//CELL_TYPE type;
    	if(inputStructure)
    	{
       		std::getline( inputStructure, strOneLine, '\n' );
       		//skip the comment at the beggining
            while ( strOneLine.find("#") != string::npos )
			{
                std::getline( inputStructure, strOneLine, '\n' );
			}
               std::getline( inputStructure, strOneLine, '\n' );
			std::stringstream oss;
			oss << strOneLine.c_str();	
			oss >> parameterName;
			oss >> parameterInt;
			if( parameterName == "SIZEX" ) 
			{
				size_x = parameterInt;
			}
			oss.clear();
            std::getline( inputStructure, strOneLine, '\n' );
			oss << strOneLine.c_str();	
			oss >> parameterName;
			oss >> parameterInt;
			if( parameterName == "SIZEY" ) 
			{
				size_y = parameterInt;
			}
			for(int j = 0; j < size_y;++j)
			{
			    std::getline( inputStructure, strOneLine, '\n' );
				oss.clear();
				oss << strOneLine.c_str();
				for(int i = 0; i < size_x;++i)
				{
					oss >> parameterInt;
					//switch (parameterInt)
					//{
					//	case 0:
					//		m_handle->m_grid->m_net[j][i]->m_type = SA_NODE;
					//		m_handle->m_grid->m_net[j][i]->m_color = QColor(255,255,0);
					//		break;
					//	case 2:
					//		//m_handle->m_grid->m_net[j][i]->m_type = ATRIAL_TISSUE;
					//		m_handle->m_grid->m_net[j][i]->m_type = ATRIAL_V3;
					//		m_handle->m_grid->m_net[j][i]->m_color = QColor(255,255,255);	
					//		break;
					//	case 1:
					//		m_handle->m_grid->m_net[j][i]->m_type = AV_NODE;
					//		m_handle->m_grid->m_net[j][i]->m_color = QColor(0,255,0);		
					//		break;
					//	case 6:
					//		m_handle->m_grid->m_net[j][i]->m_type = SOLID_WALL;
					//		m_handle->m_grid->m_net[j][i]->m_color = QColor(0,0,0);	
					//		break;							
					//	case 3:
					//		m_handle->m_grid->m_net[j][i]->m_type = PURKINIE_BUNDLE;
					//		m_handle->m_grid->m_net[j][i]->m_color = QColor(200,255,200);	
					//		break;
					//	case 4:
					//		m_handle->m_grid->m_net[j][i]->m_type = AV_NODE_BRANCH;
					//		m_handle->m_grid->m_net[j][i]->m_color = QColor(100,255,100);	
					//		break;
					//	case 5:
					//		m_handle->m_grid->m_net[j][i]->m_type = ATRIAL_BRANCH;
					//		m_handle->m_grid->m_net[j][i]->m_color = QColor(200,255,200);	
					//		break;
					//}
				}
			}
		}
		//m_handle->structurePainter->refreshPainter();
		//m_handle->structurePainter->updateGL();
	}
	inputStructure.close();
	m_handle->ui.statusBar->showMessage("Ready");
}



//----------------------------------------------
void ioHandler::saveDiffusionToFile()
{
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save Diffusion"),
        pathParameters, tr("diffusion files (*.diff)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save diffusion matrix file.\n"
		 "The path was not specified" );
        return;
	}

    pathParameters = newPath;
	ofstream output;
	output.open(pathParameters.toStdString().c_str());

	if(output.fail())
	{
		QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save diffusion matrix file.\n"
		 "Output file corrupted" );
   	}
	else
	{
		output << "# Data input file for Heart Dynamics" << endl;
		output << "# Prepared:" <<m_prefix<< endl;
		output << "# Data input file for Heart Dynamics" << endl;
		output << "" << endl;

		output << "SIZEX \t" << m_handle->m_grid->getSize()<<endl;
		output << "SIZEY \t" << m_handle->m_grid->getSize()<<endl;
		output << "LIMIT \t" << m_handle->diffusionPainter->m_upperLimit<<endl;
		for (int j = 0; j < m_handle->m_grid->getSize(); ++j)
		{
			for (int i = 0; i <  m_handle->m_grid->getSize(); ++i)
			{
				double val = m_handle->diffusionPainter->m_pixelmap->m_matrix[j][i]->color.greenF();
				val = (m_handle->diffusionPainter->m_upperLimit - m_handle->diffusionPainter->m_lowerLimit)*val +m_handle->diffusionPainter->m_lowerLimit;
				output << val << " ";
			}
			output << endl;
		}
		output << "ANISOTROPY" <<endl;
		for (int j = 0; j < m_handle->m_grid->getSize(); ++j)
		{
			for (int i = 0; i <  m_handle->m_grid->getSize(); ++i)
			{
				double val = m_handle->diffusionPainter->m_anisotrophy->m_matrix[j][i]->color.redF();
				output << val << " ";
			}
			output << endl;
		}
	}
	output.close();
	m_handle->ui.statusBar->showMessage("Diffusion and Anisotropy matrix saved");
}
////-----------------------------------------------
void ioHandler::readDiffusionFromFile()
{
	if ( QMessageBox::question( m_handle, tr("Overwrite current diffusion and anisotropy?"), tr("Overwrite current diffusion and anisotropy?"), tr("&Yes"), tr("&No"),  QString::null, 0, 1 ) )
	{
		return;
	}

    QString newPath = QFileDialog::getOpenFileName(m_handle, tr("Open diffusion matrix"), pathParameters, tr("diffusion files (*.diff)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to read diffusion matrix file.\n"
		 "The path was not specified" );
        return;
	}

    pathParameters = newPath;
	ifstream inputDiffusion;
	inputDiffusion.open(pathParameters.toStdString().c_str());

    if(inputDiffusion.fail())
    {
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save diffusion matrix file.\n"
		 "Input file corrupted" );
   	}
    else
    {
        string strOneLine = "";
		string parameterName = "";
		int size_x;
		int size_y;
		double parameterDouble = 0;
		int parameterInt = 0;
    	if(inputDiffusion)
    	{
       		std::getline( inputDiffusion, strOneLine, '\n' );
       		//skip the comment at the beggining
            while ( strOneLine.find("#") != string::npos )
			{
                std::getline( inputDiffusion, strOneLine, '\n' );
			}
               std::getline( inputDiffusion, strOneLine, '\n' );
			std::stringstream oss;
			oss << strOneLine.c_str();	
			oss >> parameterName;
			oss >> parameterInt;
			if( parameterName == "SIZEX" ) 
			{
				size_x = parameterInt;
			}
			oss.clear();
            std::getline( inputDiffusion, strOneLine, '\n' );
			oss << strOneLine.c_str();	
			oss >> parameterName;
			oss >> parameterInt;
			if( parameterName == "SIZEY" ) 
			{
				size_y = parameterInt;
			}
			oss.clear();
            std::getline( inputDiffusion, strOneLine, '\n' );
			oss << strOneLine.c_str();	
			oss >> parameterName;
			oss >> parameterDouble;
			if( parameterName == "LIMIT" ) 
			{
				m_handle->diffusionPainter->m_upperLimit = parameterDouble;
				m_handle->diffusionPainter->m_lowerLimit = 0;
				m_handle->Machine2d->m_definitions->m_maxDiff = parameterDouble;
				m_handle->setAtrialDiffusion();
				//m_handle->sb_upperLimit->setValue(parameterDouble);
			}
			for(int j = 0; j < size_y;++j)
			{
			    std::getline( inputDiffusion, strOneLine, '\n' );
				oss.clear();
				oss << strOneLine.c_str();
				for(int i = 0; i < size_x;++i)
				{
					oss >> parameterDouble;
					double xxx = parameterDouble*255/(m_handle->diffusionPainter->m_upperLimit - m_handle->diffusionPainter->m_lowerLimit)- m_handle->diffusionPainter->m_lowerLimit;
					QColor colorek;
					//		if(m_handle->m_grid->m_net[j][i]->m_type == ATRIAL_TISSUE ) colorek.setRgb(xxx,xxx,xxx);
					//		if(m_handle->m_grid->m_net[j][i]->m_type == ATRIAL_V3 ) colorek.setRgb(xxx,xxx,xxx);
					//		else if(m_handle->m_grid->m_net[j][i]->m_type == SA_NODE ) colorek.setRgb(xxx,xxx,(xxx*0.7));
					//		else if(m_handle->m_grid->m_net[j][j]->m_type == AV_NODE ) colorek.setRgb((xxx*0.7),xxx,(xxx*0.7));
					//		else if(m_handle->m_grid->m_net[j][i]->m_type == AV_NODE_BRANCH ) colorek.setRgb((xxx*0.8),xxx,(xxx*0.5));
					//		else if(m_handle->m_grid->m_net[j][i]->m_type == ATRIAL_BRANCH ) colorek.setRgb((xxx*0.7),xxx,xxx);
					//		else if(m_handle->m_grid->m_net[j][i]->m_type == PURKINIE_BUNDLE ) colorek.setRgb((xxx*0.7),(xxx*0.7),xxx);
					//		else if(m_handle->m_grid->m_net[j][i]->m_type == SOLID_WALL ) colorek.setRgb(0,0,0);
					//else colorek.setRgb(xxx,xxx,xxx);
					colorek.setRgb(xxx, xxx, xxx);
					m_handle->diffusionPainter->m_pixelmap->m_matrix[j][i]->color = (colorek);
				}
			}
			oss.clear();
            std::getline( inputDiffusion, strOneLine, '\n' );
			oss << strOneLine.c_str();	
			oss >> parameterName;
			oss >> parameterDouble;
			if( parameterName == "ANISOTROPY" ) 
			{
			}
			for(int j = 0; j < size_y;++j)
			{
			    std::getline( inputDiffusion, strOneLine, '\n' );
				oss.clear();
				oss << strOneLine.c_str();
				for(int i = 0; i < size_x;++i)
				{
					oss >> parameterDouble;
					double xxxLEFT = parameterDouble;
					QColor colorek;
					
				//	m_handle->diffusionPainter->m_grid->m_net[i][j]->m_anisotrophy_L = xxxLEFT;
				//	m_handle->diffusionPainter->m_grid->m_net[i][j]->m_anisotrophy_R = 1.0 - xxxLEFT;
				//	colorek.setRgbF(m_handle->diffusionPainter->m_grid->m_net[i][j]->m_anisotrophy_L,m_handle->diffusionPainter->m_grid->m_net[i][j]->m_anisotrophy_R,0.0);
					m_handle->diffusionPainter->m_anisotrophy->m_matrix[j][i]->color = (colorek);
				}
			}
		}
		m_handle->diffusionPainter->updateGL();
	}
	m_handle->ui.statusBar->showMessage("Diffusion matrix loaded");
	inputDiffusion.close();
}
////-----------------------------------------------
void ioHandler::savePotentialPlot()
{
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save potential vs. time plots"),
        pathParameters, tr("potential plot files (*.pplot)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save potential vs. time plot.\n"
		 "The path was not specified" );
        return;
	}
    pathParameters = newPath;

	ofstream output;
	output.open(pathParameters.toStdString().c_str());
	if(output.fail())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save potential vs. time plot.\n"
		 "Output file corrupted" );
   	}
	else
	{
		output << "# Potential plot file for Simple Heart" << endl;
		output << "# Prepared:" <<m_prefix<< endl;
		output << "# Prepared by PP" << endl;
		output << "" << endl;

		output << "Electrode 1 position (X,Y): \t ( " << m_handle->Machine2d->probeOscillator[0]->getPositionX()<<", ";
		output <<  m_handle->Machine2d->probeOscillator[0]->getPositionY()<<") "<<endl;
		output << "Electrode 2 position (X,Y): \t ( " << m_handle->Machine2d->probeOscillator[1]->getPositionX()<<", ";
		output <<  m_handle->Machine2d->probeOscillator[1]->getPositionY()<<") "<<endl;
		output << "Electrode 3 position (X,Y): \t ( " << m_handle->Machine2d->probeOscillator[2]->getPositionX()<<", ";
		output <<  m_handle->Machine2d->probeOscillator[2]->getPositionY()<<") "<<endl;
		output << "time \t v[electr 1] \t v[electr_2] \t v[electr3]" <<endl <<endl;


		for (int j = 0; j < m_handle->plotPotentialE1->d_data[0]->size(); ++j)
		{
			output << m_handle->plotPotentialE1->d_data[0]->x()[j]  << "\t  ";
			output << m_handle->plotPotentialE1->d_data[0]->y()[j]  << "\t  ";
			output << m_handle->plotPotentialE2->d_data[1]->y()[j] << " \t ";
			output << m_handle->plotPotentialE3->d_data[2]->y()[j]  << " ";
			output << endl;
		}
	}
	output.close();
	m_handle->ui.statusBar->showMessage("Potential data  saved");
	///TODO - save log here also
}
//---------------------------------------------
void ioHandler::saveRRPlot_1()
{
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save interspike intervals plot 1"),
        pathParameters, tr("interspike intervals plot (*.isi)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save interspike intervals plot 1.\n"
		 "The path was not specified" );
        return;
	}
    pathParameters = newPath;

	ofstream output;
	output.open(pathParameters.toStdString().c_str());
	if(output.fail())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save interspike intervals plot 1.\n"
		 "Output file corrupted" );
   	}
	else
	{
		output << "# Interspike intervals plot file for Heart Dynamics" << endl;
		output << "# Prepared:" <<m_prefix<< endl;
		output << "# Prepared by PP" << endl;
		output << "" << endl;
		output << "ISI for electrode 1" << endl;
		output << "Electrode 1 position (X,Y): \t ( " << m_handle->Machine2d->probeOscillator[0]->getPositionX()<<", ";
		output <<  m_handle->Machine2d->probeOscillator[0]->getPositionY()<<") "<<endl;

		for (int j = 0; j < m_handle->plotRR_el1->d_data[0]->size(); ++j)
		{
			output << m_handle->plotRR_el1->d_data[0]->x()[j]  << "\t  ";
			output << m_handle->plotRR_el1->d_data[0]->y()[j]  << " ";
			output << endl;
		}
	}
	output.close();
	m_handle->ui.statusBar->showMessage("ISI plot 1 saved");
	///TODO - save log here also
}
void ioHandler::saveRRPlot_2()
{
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save interspike intervals plot 2"),
        pathParameters, tr("interspike intervals plot (*.isi)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save interspike intervals plot 2.\n"
		 "The path was not specified" );
        return;
	}
    pathParameters = newPath;

	ofstream output;
	output.open(pathParameters.toStdString().c_str());
	if(output.fail())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save interspike intervals plot 2.\n"
		 "Output file corrupted" );
   	}
	else
	{
		output << "# Interspike intervals plot file for Heart Dynamics" << endl;
		output << "# Prepared:" <<m_prefix<< endl;
		output << "# Prepared by PP" << endl;
		output << "" << endl;
		output << "ISI for electrode 2" << endl;
		output << "Electrode 2 position (X,Y): \t ( " << m_handle->Machine2d->probeOscillator[1]->getPositionX()<<", ";
		output <<  m_handle->Machine2d->probeOscillator[1]->getPositionY()<<") "<<endl;

		for (int j = 0; j < m_handle->plotRR_el2->d_data[0]->size(); ++j)
		{
			output << m_handle->plotRR_el2->d_data[0]->x()[j]  << "\t  ";
			output << m_handle->plotRR_el2->d_data[0]->y()[j]  << " ";
			output << endl;
		}
	}
	output.close();
	m_handle->ui.statusBar->showMessage("ISI plot 2 saved");
	///TODO - save log here also
}
void ioHandler::saveRRPlot_3()
{
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save interspike intervals plot 3"),
        pathParameters, tr("interspike intervals plot (*.isi)"));

    if (newPath.isEmpty())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save interspike intervals plot 3.\n"
		 "The path was not specified" );
        return;
	}
    pathParameters = newPath;

	ofstream output;
	output.open(pathParameters.toStdString().c_str());
	if(output.fail())
	{
	    QMessageBox::information( m_handle, "Simple Heart",
	    "Unable to save interspike intervals plot 3.\n"
		 "Output file corrupted" );
   	}
	else
	{
		output << "# Interspike intervals plot file for Heart Dynamics" << endl;
		output << "# Prepared:" <<m_prefix<< endl;
		output << "# Prepared by PP" << endl;
		output << "" << endl;
		output << "ISI for electrode 3" << endl;
		output << "Electrode 3 position (X,Y): \t ( " << m_handle->Machine2d->probeOscillator[2]->getPositionX()<<", ";
		output <<  m_handle->Machine2d->probeOscillator[2]->getPositionY()<<") "<<endl;

		for (int j = 0; j < m_handle->plotRR_el3->d_data[0]->size(); ++j)
		{
			output << m_handle->plotRR_el3->d_data[0]->x()[j]  << "\t  ";
			output << m_handle->plotRR_el3->d_data[0]->y()[j]  << " ";
			output << endl;
		}
	}
	output.close();
	m_handle->ui.statusBar->showMessage("ISI plot 3 saved");
	///TODO - save log here also
}









string ioHandler::filePrefix()
{
    stringstream version;
    struct tm *current;
	time_t now;
	time(&now);
	current = localtime(&now);
	version << current->tm_mday<<"_"<<current->tm_mon<<"_"<< current->tm_hour<<"h"<< current->tm_min<<"m"<< current->tm_sec<<"s";

    return version.str();
}
//------------------------------------------------
string ioHandler::intToStr(int n)
{
     string tmp, ret;
     if(n < 0) {
          ret = "-";
          n = -n;
     }
     do {
          tmp += n % 10 + 48;
          n -= n % 10;
     }
     while(n /= 10);
     for(int i = tmp.size(); i >= 0; i--)
          ret += tmp[i];
     return ret;
};
//------------------------------------------------
int ioHandler::strToInt(string s)
{
     int tmp = 0, i = 0;
     bool m = false;
     if(s[0] == '-') {
          m = true;
          i++;
     }
     for(; i < s.size(); i++)
          tmp = 10 * tmp + s[i] - 48;
     return m ? -tmp : tmp;   
}

//
//bool ioHandler::saveCurrentStructure()
//{
//	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save current model structure"),
//		pathParameters, tr("MATLAB FILES (*.mat)"));
//
//	if (newPath.isEmpty())
//	{
//		QMessageBox::information(m_handle, "Simple Heart", "Unable to save current model structure to file.\n"
//			"The path and filename were not specified correctly");
//		return false;
//	}
//
//	pathParameters = newPath;
//	QByteArray ba = pathParameters.toLatin1();
//	const char *outname = ba.data();
//
//
//	//[0] Prepare matrix sizes
//	const int numberOfColumns = 11;
//	const int meshSize = m_handle->m_grid->m_mesh.size();
//
//	//[1] Prepare and fill description of data in 1d string array form
//	char* matnames[numberOfColumns];
//	matnames[0] = "ID";
//	matnames[1] = "x [mm]";
//	matnames[2] = "y [mm]";
//	matnames[3] = "z [mm]";
//	matnames[4] = "TYPE";
//	matnames[5] = "neighbour 1 ID";
//	matnames[6] = "neighbour 2 ID";
//	matnames[7] = "neighbour 3 ID";
//	matnames[8] = "neighbour 4 ID";
//	matnames[9] = "neighbour 5 ID";
//	matnames[10] = "neighbour 6 ID";
//
//	//[2] Prepare data in 2d double array form
//	double *matvalues;
//	matvalues = (double *)malloc(sizeof(double) * numberOfColumns * meshSize);
//
//
//	//[3] Fill data to 2d double array form
//	int type;
//	Oscillator* osc;
//	for (int i = 0; i < meshSize; ++i) {
//		osc = m_handle->m_grid->m_mesh[i];
//		matvalues[meshSize * 0 + i] = static_cast<double> (osc->oscillatorID);
//		matvalues[meshSize * 1 + i] = osc->m_x;
//		matvalues[meshSize * 2 + i] = osc->m_y;
//		matvalues[meshSize * 3 + i] = osc->m_z;
//		type = osc->m_type;
//		matvalues[meshSize * 4 + i] = static_cast<double> (type);
//		for (int currentNeighbour = 0; currentNeighbour < osc->m_neighbours.size(); ++currentNeighbour)
//		{
//			matvalues[meshSize * (5 + currentNeighbour) + i] = static_cast<double> (osc->m_neighbours[currentNeighbour]->oscillatorID);
//		}
//		for (int fillToEnd = 5 + osc->m_neighbours.size(); fillToEnd < numberOfColumns; ++fillToEnd)
//		{
//			matvalues[meshSize * fillToEnd + i] = static_cast<double> (-1);
//		}
//	}
//
//	//[4] Setup the output variables
//	mat_t *mat = Mat_CreateVer(outname, NULL, MAT_FT_DEFAULT);
//	matvar_t *matvar;
//	matvar_t *cell_array, *cell_element;
//	size_t dims1[2] = { numberOfColumns, 1 };
//	size_t dims2[2] = { meshSize, numberOfColumns };
//	size_t dims[2];
//
//	//[5] Save to mat file
//	if (mat)
//	{
//		////[5.1] Save the first variable matnames
//		//cell_array = Mat_VarCreate("data_description", MAT_C_CELL, MAT_T_CELL, 2, dims1, NULL, 0);
//		//dims[1] = 1;
//		//for (int k = 0; k < numberOfColumns; ++k)
//		//{
//		//	dims[0] = strlen(matnames[k]);
//		//	char * str = matnames[k];
//		//	cell_element = Mat_VarCreate(NULL, MAT_C_CHAR, MAT_T_UINT8, 2, dims, str, 0);
//		//	Mat_VarSetCell(cell_array, k, cell_element);
//		//}
//		//Mat_VarWrite(mat, cell_array, MAT_COMPRESSION_ZLIB);
//		//Mat_VarFree(cell_array);
//
//		//[5.2] Save the second variable matvalues
//		matvar = Mat_VarCreate(VARNAME_MODEL_DATA, MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
//			dims2, matvalues, 0);
//		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
//		Mat_VarFree(matvar);
//
//		//[5.3] Close output
//		Mat_Close(mat);
//	}
//	else
//	{
//		return false;
//	}
//	free(matvalues);
//
//	return true;
//}



//matvar = Mat_VarRead(matfp, "oscillator_ID");
//if (NULL == matvar) {
//	QMessageBox::information(m_handle, "Simple Heart", "Model data not found, or error reading MAT file"); return nullptr;
//}
//int meshSize = matvar->dims[0];
//INT32 *oscillator_ID = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
//oscillator_ID = static_cast<INT32*>(matvar->data);