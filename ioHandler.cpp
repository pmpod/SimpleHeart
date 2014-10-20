#include "ioHandler.h"
#include "simpleheart.h"

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