#include "ioHandler.h"
#include "simpleheart.h"
#include <stdlib.h>

#define VARNAME_MODEL_DATA "mesh"
#define VARNAME_DESC_DATA "data_description"

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



//------------------------------------------------
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
mat_t * ioHandler::initMatlabLoad(const char *inname)
{
	mat_t *matfp;
	matfp = Mat_Open(inname, MAT_ACC_RDONLY);
	if (NULL == matfp) {
		QMessageBox::information(m_handle, "Simple Heart", "Error opening MAT file"); return nullptr;
	}
	return matfp;
}
//------------------------------------------------
// Structure save and load functions -------------
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
	double *sigma = (double *)malloc(sizeof(double) * 1 * meshSize);
	double *erp_parameter = (double *)malloc(sizeof(double) * 1 * meshSize);
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
		sigma[i] = osc->m_sigmaX;
		erp_parameter[i] = osc->getERP();

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
		//[5.1] Save variables to  matvalues
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

		dims[1] = 1;
		matvar = Mat_VarCreate("sigma", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, sigma, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		matvar = Mat_VarCreate("erp_parameter", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, erp_parameter, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		//[5.2] Close output
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
	free(erp_parameter);
	free(sigma);

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

	return CardiacMesh::importGrid(inname);

	m_handle->glGraph->updateGL();
	m_handle->reset();
	m_handle->Machine2d->reset();
}
//------------------------------------------------
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

	//[3] Read matlab variable probeElectrodes
	INT32 *electrodes_ID = nullptr;
	matvar = readMatVariable("probeElectrodes", matfp, electrodes_ID);

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
	for (int i = 0; i < m_handle->Machine2d->stimulator->probeElectrodesCount(); ++i)
	{
		int oscID = electrodes_ID[i];
		//osc = m_handle->m_grid->m_mesh[oscID];
		//m_handle->Machine2d->probeOscillator[i]->setOscillator(osc);
		m_handle->Machine2d->stimulator->setProbeElectrode(m_handle->m_grid, i, oscID);
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
	const int numberOfProbes = m_handle->Machine2d->stimulator->probeElectrodesCount();
	//[2] Prepare data arrays in form accepted by MAT
	double *position_xyz  = (double *)malloc(sizeof(double) * 3 * meshSize);
	INT32  *oscillator_ID = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);
	double *potential     = (double *)malloc(sizeof(double) * 1 * meshSize);
	//TODO Handling multiple cell types with different variables number
	double *variables     = (double *)malloc(sizeof(double) *  m_handle->m_grid->m_mesh[0]->getNumberOfCurrents() * meshSize);
	double *electrogram   = (double *)malloc(sizeof(double) * 1 * meshSize);
	double *csd           = (double *)malloc(sizeof(double) * 1 * meshSize);
	double *time_ms       = (double *)malloc(sizeof(double) * 1);
	double *ppir_tcl	  =	(double *)malloc(sizeof(double) * 1 * meshSize);
	double *tcl		      = (double *)malloc(sizeof(double) * 1);



	INT32  *electrodes_ID = (INT32 *)malloc(sizeof(INT32) * 1 * numberOfProbes);
	char description[] = "One frame data pack";


	//[3] Fill data to 2d double array form
	int type;
	Oscillator* osc;

	time_ms[0] = m_handle->m_grid->m_simulationTime;
	tcl[0] = EpStimulator::Instance()->referenceTCLforPPIR();
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
		ppir_tcl[i] = osc->m_lastPPIR_TCL;
	}
	for (int i = 0; i < numberOfProbes; ++i)
	{
		electrodes_ID[i] = m_handle->Machine2d->stimulator->probeElectrodeID(i);
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
		dims[0] = meshSize;
		matvar = Mat_VarCreate("position_xyz", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, position_xyz, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("oscillator_ID", MAT_C_INT32, MAT_T_INT32, 2,
			dims, oscillator_ID, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("potential", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, potential, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = m_handle->m_grid->m_mesh[0]->getNumberOfCurrents(); //TODO multiple number of currents
		dims[0] = meshSize;
		matvar = Mat_VarCreate("currVariables", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, variables, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("electrogram", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, electrogram, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("curerntSourceDensity", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, csd, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("PPIR_TCL", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, ppir_tcl, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = 1;
		matvar = Mat_VarCreate("time_ms", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, time_ms, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = 1;
		matvar = Mat_VarCreate("TCL", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, tcl, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = numberOfProbes;
		matvar = Mat_VarCreate("probeElectrodes", MAT_C_INT32, MAT_T_INT32, 2,
			dims, electrodes_ID, 0);
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
	free(electrodes_ID);
	free(ppir_tcl);
	free(tcl);

	return true;
}
//------------------------------------------------

bool ioHandler::saveElectrodeGrid()
{

	//[0] Creat proper filename
	QString newPath = QFileDialog::getSaveFileName(m_handle, tr("Save current electrode grid state. The path should not contain Polish letters!"), pathParameters, tr("MATLAB FILES (*.mat)"));
	if (newPath.isEmpty())
	{
		QMessageBox::information(m_handle, "Simple Heart", "Unable to save currentsimulation state.\n"
			"The path and filename were not specified correctly");
		return false;
	}
	pathParameters = newPath;
	QByteArray ba = pathParameters.replace(" ", "\x20").toLatin1();
	const char *outname = ba.data();


	m_handle->m_grid->gridElectrode->calculateHilbert();
	//[1] Prepare matrix sizes
	const int meshSize = m_handle->m_grid->gridElectrode->_oscMatrix.size();
	const int signalSize = m_handle->m_grid->gridElectrode->m_historyElectrogram[0].size();

	//[2] Prepare data arrays in form accepted by MAT
	double *electrogram = (double *)malloc(sizeof(double) * signalSize * meshSize);
	double *potential = (double *)malloc(sizeof(double) * signalSize * meshSize);
	double *phase = (double *)malloc(sizeof(double) * signalSize * meshSize);

	double *time_ms = (double *)malloc(sizeof(double) * signalSize * meshSize);

	double *position_absolute = (double *)malloc(sizeof(double) * 3 * meshSize);
	double *position_relative = (double *)malloc(sizeof(double) * 3 * meshSize);
	INT32  *oscillator_ID = (INT32 *)malloc(sizeof(INT32) * 1 * meshSize);

	double *electrode_spacing = (double *)malloc(sizeof(double) * 1);
	int *electrode_grid_size_X = (INT32 *)malloc(sizeof(INT32) * 1);
	int *electrode_grid_size_Y = (INT32 *)malloc(sizeof(INT32) * 1);
	//TODO Handling multiple cell types with different variables number

	char description[] = "Grid electrode data pack";

	//[3] Fill data to 2d double array form
	int type;
	Oscillator* osc;

	for (int i = 0; i < meshSize; ++i)
	{
		osc = m_handle->m_grid->m_mesh[m_handle->m_grid->gridElectrode->_oscMatrix[i]];
		position_absolute[meshSize * 0 + i] = osc->m_x;
		position_absolute[meshSize * 1 + i] = osc->m_y;
		position_absolute[meshSize * 2 + i] = osc->m_z;

		position_relative[meshSize * 0 + i] = m_handle->m_grid->gridElectrode->_spacingXY*static_cast<double>(i % m_handle->m_grid->gridElectrode->_numberOfElectrodesX);
		position_relative[meshSize * 1 + i] = m_handle->m_grid->gridElectrode->_spacingXY*floor(static_cast<double>(i) / static_cast<double>(m_handle->m_grid->gridElectrode->_numberOfElectrodesY));
		position_relative[meshSize * 2 + i] = 0;
		oscillator_ID[i] = osc->oscillatorID;

		for (int j = 0; j < signalSize; ++j)
		{
			potential[signalSize * i + j] = m_handle->m_grid->gridElectrode->m_historyPotential[i][j];
			electrogram[signalSize * i + j] = m_handle->m_grid->gridElectrode->m_historyElectrogram[i][j];
			phase[signalSize * i + j] = m_handle->m_grid->gridElectrode->m_historyAfterHilbert[i][j];
			time_ms[signalSize * i + j] = m_handle->m_grid->gridElectrode->m_historyTime[i][j];
		}
	}

	electrode_spacing[0] = m_handle->m_grid->gridElectrode->_spacingXY;
	electrode_grid_size_X[0] = m_handle->m_grid->gridElectrode->_numberOfElectrodesX;
	electrode_grid_size_Y[0] = m_handle->m_grid->gridElectrode->_numberOfElectrodesY;
	//[4] Setup the output variables
	mat_t *mat = Mat_CreateVer(outname, NULL, MAT_FT_DEFAULT);
	matvar_t *matvar;
	size_t dims[2];
	dims[0] = meshSize;

	//[5] Save to mat file
	if (mat)
	{
		//[5.2] Save the second variable matvalues


		dims[1] = 1;
		dims[0] = 1;
		matvar = Mat_VarCreate("electrode_grid_size_X", MAT_C_INT32, MAT_T_INT32, 2,
			dims, electrode_grid_size_X, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = 1;
		matvar = Mat_VarCreate("electrode_grid_size_Y", MAT_C_INT32, MAT_T_INT32, 2,
			dims, electrode_grid_size_Y, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = 1;
		matvar = Mat_VarCreate("electrode_spacing", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, electrode_spacing, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 3;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("position_absolute", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, position_absolute, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 3;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("position_relative", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, position_relative, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = 1;
		dims[0] = meshSize;
		matvar = Mat_VarCreate("oscillator_ID", MAT_C_INT32, MAT_T_INT32, 2,
			dims, oscillator_ID, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = meshSize;
		dims[0] = signalSize;
		matvar = Mat_VarCreate("time", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, time_ms, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = meshSize;
		dims[0] = signalSize;
		matvar = Mat_VarCreate("unipolar_potential", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, potential, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);

		dims[1] = meshSize;
		dims[0] = signalSize;
		matvar = Mat_VarCreate("electrogram", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, phase, 0);
		Mat_VarWrite(mat, matvar, MAT_COMPRESSION_ZLIB);
		Mat_VarFree(matvar);
		

		dims[1] = meshSize;
		dims[0] = signalSize;
		matvar = Mat_VarCreate("hilbert_phase", MAT_C_DOUBLE, MAT_T_DOUBLE, 2,
			dims, electrogram, 0);
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
	free(position_absolute);
	free(position_relative);
	free(potential);
	free(electrogram);
	free(phase);
	free(time_ms);
	free(oscillator_ID);
	free(electrode_grid_size_X);
	free(electrode_grid_size_Y);
	free(electrode_spacing);

	return true;

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
			outputParameters << "maxdiff \t"<< '0'<<endl;
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
	//inputParameters.open(paramFilename.c_str());
 //   if(inputParameters.fail())
 //   {
	//	QMessageBox::information(m_handle, tr("Base"),
	//	 "+++ ERROR!!! The file does not exist.... +++ ");
 //  	}
 //   else
 //   {
 //       string strOneLine = "";
	//	string parameterName = "";
	//	double parameterValue = 0;

 //   	while (inputParameters)
 //   	{
 //      		std::getline( inputParameters, strOneLine, '\n' );
 //      		//skip the comment at the beggining
 //           while ( strOneLine.find("#") != string::npos )
 //           {
 //               std::getline( inputParameters, strOneLine, '\n' );
 //           }

 //           stringstream oss;
	//		//QMessageBox::information(this, tr("Base"),
	//		//				   tr(strOneLine.c_str()));
	//		oss.clear();
	//			if(strOneLine == "SIMULATION")
	//			{
	//				std::getline( inputParameters, strOneLine, '\n' );
	//				oss.clear();
	//				oss << strOneLine.c_str();
	//				while(strOneLine != "!END")
	//				{
	//					oss >> parameterName;
	//					oss >> parameterValue;

	//					if( parameterName == "step" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setMainTimeStep(parameterValue);
	//						//m_handle->Machine2d->setMainTimeStep(parameterValue);
	//					}
	//					else if( parameterName == "skip" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSkip(parameterValue);
	//						m_handle->Machine2d->setSkip(parameterValue);
	//					}
	//					else if( parameterName == "skip" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSkip(parameterValue);
	//						m_handle->Machine2d->setSkip(parameterValue);
	//					}
	//					else if( parameterName == "maxdiff" ) 
	//					{
	//						//m_handle->diffusionPainter->m_upperLimit = parameterValue;
	//						//m_handle->diffusionPainter->m_lowerLimit = 0;
	//						m_handle->Machine2d->m_definitions->m_maxDiff = parameterValue;
	//						//m_handle->setAtrialDiffusion();
	//					}
	//					else if( parameterName == "ectosizeX" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_ectopicSizeX=parameterValue;
	//					}
	//					else if( parameterName == "ectosizeY" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_ectopicSizeY=parameterValue;
	//					}
	//					else if( parameterName == "ectoamp" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_ectopicAmplitude=parameterValue;
	//					}
	//					else if( parameterName == "ectoperiodS1" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_ectopicPeriodS1=parameterValue;
	//						m_handle->ui.sb_simPeriod->setValue(parameterValue);
	//					}
	//					else if( parameterName == "ectoperiodS2" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_ectopicPeriodS2=parameterValue;
	//						m_handle->ui.sb_simPeriod_2->setValue(parameterValue);
	//					}
	//					else if( parameterName == "ectolength" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_ectopicLength=parameterValue;
	//					}
	//					else if( parameterName == "tau" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_tau=parameterValue;
	//						//m_handle->ui.sb_tau->setValue(parameterValue);
	//					}
	//					else if( parameterName == "binsize" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_binSize=parameterValue;
	//						//m_handle->ui.sb_binSize->setValue(parameterValue);
	//					}
	//					else if( parameterName == "winsize" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->m_windowSize=parameterValue;
	//						//m_handle->ui.sb_winSize->setValue(parameterValue);
	//					}
	//					std::getline( inputParameters, strOneLine, '\n' );
	//					oss.clear();
	//					oss << strOneLine;
	//				}
	//			}
	//			if(strOneLine == "SA_NODE")
	//			{
	//				std::getline( inputParameters, strOneLine, '\n' );
	//				oss.clear();
	//				oss << strOneLine.c_str();
	//				while(strOneLine != "!END")
	//				{
	//					oss >> parameterName;
	//					oss >> parameterValue;

	//					if( parameterName == "alpha" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalAlpha(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalAlpha(parameterValue);
	//					}
	//					else if( parameterName == "v1" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalV1(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalV1(parameterValue);
	//					}
	//					else if( parameterName == "v2" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalV2(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalV2(parameterValue);
	//					}
	//					else if( parameterName == "d" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalD(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalD(parameterValue);
	//					}
	//					else if( parameterName == "e" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalE(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalE(parameterValue);
	//					}
	//					else if( parameterName == "f" )
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalF(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalF(parameterValue);
	//					}						
	//					//	else if( parameterValue == "deltax" )
	//					//	else if( parameterValue == "deltax" )
	//					std::getline( inputParameters, strOneLine, '\n' );
	//					oss.clear();
	//					oss << strOneLine;
	//				}
	//			}
	//			if(strOneLine == "SA_NODE_2")
	//			{
	//				std::getline( inputParameters, strOneLine, '\n' );
	//				oss.clear();
	//				oss << strOneLine.c_str();
	//				while(strOneLine != "!END")
	//				{
	//					oss >> parameterName;
	//					oss >> parameterValue;

	//					if( parameterName == "alpha" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalAlpha_2(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalAlpha_2(parameterValue);
	//					}
	//					else if( parameterName == "v1" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalV1_2(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalV1_2(parameterValue);
	//					}
	//					else if( parameterName == "v2" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalV2_2(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalV(parameterValue);
	//					}
	//					else if( parameterName == "d" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalD_2(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalD_2(parameterValue);
	//					}
	//					else if( parameterName == "e" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalE_2(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalE(parameterValue);
	//					}
	//					else if( parameterName == "f" )
	//					{
	//						m_handle->Machine2d->m_definitions->setSaGlobalF_2(parameterValue);
	//						//m_handle->Machine2d->setSaGlobalF(parameterValue);
	//					}						
	//					//	else if( parameterValue == "deltax" )
	//					//	else if( parameterValue == "deltax" )
	//					std::getline( inputParameters, strOneLine, '\n' );
	//					oss.clear();
	//					oss << strOneLine;
	//				}
	//			}
	//			else if(strOneLine == "ATRIUM")
	//			{
	//					std::getline( inputParameters, strOneLine, '\n' );
	//					oss.clear();
	//					oss << strOneLine;

	//				while(strOneLine != "!END")
	//				{
	//					oss >> parameterName;
	//					oss >> parameterValue;
	//					if( parameterName == "beta" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAtriumGlobalBeta(parameterValue);
	//						//m_handle->Machine2d->setAtriumGlobalBeta(parameterValue);
	//					}
	//					else if( parameterName == "gamma" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAtriumGlobalGamma(parameterValue);
	//					//	m_handle->Machine2d->setAtriumGlobalGamma(parameterValue);
	//					}
	//					else if( parameterName == "ni" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAtriumGlobalNi(parameterValue);
	//						//m_handle->Machine2d->setAtriumGlobalNi(parameterValue);
	//						m_handle->ui.m_refSlider->setValue(static_cast<int>((100-parameterValue*1000)));
	//						m_handle->ui.m_refraction->setValue(static_cast<int>((100-parameterValue*1000)));

	//					}
	//					else if( parameterName == "alpha" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAtriumGlobalAlpha(parameterValue);
	//					//	m_handle->Machine2d->setAtriumGlobalAlpha(parameterValue);
	//					}
	//					else if( parameterName == "C1" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAtriumGlobalC1(parameterValue);
	//					//	m_handle->Machine2d->setAtriumGlobalC1(parameterValue);
	//					}
	//					else if( parameterName == "C2" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAtriumGlobalC2(parameterValue);
	//					//	m_handle->Machine2d->setAtriumGlobalC2(parameterValue);
	//					}
	//					//	else if( parameterValue == "deltax" )
	//					//	else if( parameterValue == "deltax" )
	//					std::getline( inputParameters, strOneLine, '\n' );
	//					oss.clear();
	//					oss << strOneLine;
	//				}
	//			}
	//			else if(strOneLine == "AV_NODE")
	//			{
	//					std::getline( inputParameters, strOneLine, '\n' );
	//					oss << strOneLine;
	//				while(strOneLine != "!END")
	//				{
	//					oss >> parameterName;
	//					oss >> parameterValue;
	//					if( parameterName == "alpha" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAvGlobalAlpha(parameterValue);
	//					//	m_handle->Machine2d->setAvGlobalAlpha(parameterValue);
	//					}
	//					else if( parameterName == "v1" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAvGlobalV1(parameterValue);
	//					//	m_handle->Machine2d->setAvGlobalV1(parameterValue);
	//					}
	//					else if( parameterName == "v2" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAvGlobalV2(parameterValue);
	//					//	m_handle->Machine2d->setAvGlobalV2(parameterValue);
	//					}
	//					else if( parameterName == "d" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAvGlobalD(parameterValue);
	//					//	m_handle->Machine2d->setAvGlobalD(parameterValue);
	//					}
	//					else if( parameterName == "e" ) 
	//					{
	//						m_handle->Machine2d->m_definitions->setAvGlobalE(parameterValue);
	//					//	m_handle->Machine2d->setAvGlobalE(parameterValue);
	//					}
	//					else if( parameterName == "f" )
	//					{
	//						m_handle->Machine2d->m_definitions->setAvGlobalF(parameterValue);
	//					//	m_handle->Machine2d->setAvGlobalF(parameterValue);
	//					}						
	//					//	else if( parameterValue == "deltax" )
	//					//	else if( parameterValue == "deltax" )
	//					std::getline( inputParameters, strOneLine, '\n' );
	//					oss.clear();
	//					oss << strOneLine;
	//				}
	//			}
	//			else
	//			{
	//			}
	//		}
	//}
	//inputParameters.close();
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


		int oscID = m_handle->Machine2d->stimulator->probeElectrodeID(0);
		output << "Electrode 1 position (X,Y): \t ( " << m_handle->m_grid->m_mesh[oscID]->getPositionX() << ", ";
		output << m_handle->m_grid->m_mesh[oscID]->getPositionY() << ") " << endl;

		oscID = m_handle->Machine2d->stimulator->probeElectrodeID(1);
		output << "Electrode 2 position (X,Y): \t ( " << m_handle->m_grid->m_mesh[oscID]->getPositionX() << ", ";
		output << m_handle->m_grid->m_mesh[oscID]->getPositionY() << ") " << endl;

		oscID = m_handle->Machine2d->stimulator->probeElectrodeID(2);
		output << "Electrode 3 position (X,Y): \t ( " << m_handle->m_grid->m_mesh[oscID]->getPositionX()<<", ";
		output << m_handle->m_grid->m_mesh[oscID]->getPositionY() << ") " << endl;
		output << "time \t v[electr 1] \t v[electr_2] \t v[electr3]" <<endl <<endl;


		for (int j = 0; j < m_handle->plotPotentials[0]->d_data[0]->size(); ++j)
		{
			output << m_handle->plotPotentials[0]->d_data[0]->x()[j]  << "\t  ";
			output << m_handle->plotPotentials[0]->d_data[0]->y()[j]  << "\t  ";
			output << m_handle->plotPotentials[1]->d_data[1]->y()[j] << " \t ";
			output << m_handle->plotPotentials[2]->d_data[2]->y()[j]  << " ";
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
		int oscID = m_handle->Machine2d->stimulator->probeElectrodeID(0);
		output << "Electrode 1 position (X,Y): \t ( " << m_handle->m_grid->m_mesh[oscID]->getPositionX() << ", ";
		output << m_handle->m_grid->m_mesh[oscID]->getPositionY() << ") " << endl;

		for (int j = 0; j < m_handle->plotRR[0]->d_data[0]->size(); ++j)
		{
			output << m_handle->plotRR[0]->d_data[0]->x()[j]  << "\t  ";
			output << m_handle->plotRR[0]->d_data[0]->y()[j]  << " ";
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
		int oscID = m_handle->Machine2d->stimulator->probeElectrodeID(1);
		output << "Electrode 1 position (X,Y): \t ( " << m_handle->m_grid->m_mesh[oscID]->getPositionX() << ", ";
		output << m_handle->m_grid->m_mesh[oscID]->getPositionY() << ") " << endl;

		for (int j = 0; j < m_handle->plotRR[1]->d_data[0]->size(); ++j)
		{
			output << m_handle->plotRR[1]->d_data[0]->x()[j] << "\t  ";
			output << m_handle->plotRR[1]->d_data[0]->y()[j] << " ";
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
		int oscID = m_handle->Machine2d->stimulator->probeElectrodeID(2);
		output << "Electrode 1 position (X,Y): \t ( " << m_handle->m_grid->m_mesh[oscID]->getPositionX() << ", ";
		output << m_handle->m_grid->m_mesh[oscID]->getPositionY() << ") " << endl;

		for (int j = 0; j < m_handle->plotRR[2]->d_data[0]->size(); ++j)
		{
			output << m_handle->plotRR[2]->d_data[0]->x()[j] << "\t  ";
			output << m_handle->plotRR[2]->d_data[0]->y()[j] << " ";
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