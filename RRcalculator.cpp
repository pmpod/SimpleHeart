#include "RRcalculator.h"

RRcalculator::RRcalculator(atrialParameters* definitions)
{
	m_definitions = definitions;
	rangeOffocus = 5;
	lastMaxRR = -1;
	currentMaxRR = -1;
	maxRRindex = -1;
	m_definitions->m_tau = 2;
	m_definitions->m_windowSize = 50;
	m_definitions->m_binSize = 5;
}

void RRcalculator::processNewTime(double time, double potential)
{
	intervalsRR.push_back(time);
	intervalsPotential.push_back(potential);
	if(intervalsRR.size() > 2*rangeOffocus)
	{
		intervalsRR.erase (intervalsRR.begin());
		intervalsPotential.erase (intervalsPotential.begin());
	}
	if(potential>0.3)
	{
		for (int i=1; i<intervalsPotential.size(); ++i)
		{
			if( intervalsPotential[i] >= intervalsPotential[i-1]*(1+0.001) )
			{
				maxRRindex = i;
				currentMaxRR = intervalsRR[i];
			}
		}
		if( maxRRindex == rangeOffocus && currentMaxRR != -1 && (currentMaxRR > 0) && (currentMaxRR - lastMaxRR > 50))
		{
			//
			double outer = currentMaxRR - lastMaxRR;
			emit nextRR(outer);
			intervalsSaved.push_back(outer);
			lastMaxRR = currentMaxRR;
		}	
		emit curIndex(maxRRindex);
		currentMaxRR = -1;
	}
}
RRcalculator::~RRcalculator(void)
{
}

void RRcalculator::reset()
{
	intervalsSaved.clear();
	intervalsRR.clear();
	intervalsPotential.clear();
		rangeOffocus = 5;
	lastMaxRR = -1;
	currentMaxRR = -1;
	maxRRindex = -1;
}


void RRcalculator::calculatePatEn(double rr)
{



	if( intervalsSaved.size() > (m_definitions->m_windowSize + 2*m_definitions->m_tau) )
	{

		double currentWindow;
		m_histogram_0.clear();
		m_histogram_1t.clear();
		m_histogram_2t.clear();
		int upperBound = m_definitions->m_windowSize;

		currentWindow = upperBound;

	//ROBIMY HISTOGRAMY
			for (int j = intervalsSaved.size()-(2*m_definitions->m_tau); j > intervalsSaved.size()-(2*m_definitions->m_tau)-upperBound; --j)
			{
				int temp;
				temp = static_cast<int>(intervalsSaved[j]/m_definitions->m_binSize);
				while (temp >= m_histogram_0.size())
				{
					m_histogram_0.push_back(0);
				}
				while (temp >= m_histogram_1t.size())
				{
					m_histogram_1t.push_back(0);
				}
				while (temp >= m_histogram_2t.size())
				{
					m_histogram_2t.push_back(0);
				}
				m_histogram_0[temp]+=1.0;
			}
			for (int j = intervalsSaved.size()-(m_definitions->m_tau); j > intervalsSaved.size()-(m_definitions->m_tau)-upperBound; --j)
			{
				int temp;
				temp = static_cast<int>(intervalsSaved[j]/m_definitions->m_binSize);
				while (temp >= m_histogram_0.size())
				{
					m_histogram_0.push_back(0);
				}
				while (temp >= m_histogram_1t.size())
				{
					m_histogram_1t.push_back(0);
				}
				while (temp >= m_histogram_2t.size())
				{
					m_histogram_2t.push_back(0);
				}
				m_histogram_1t[temp]+=1.0;
			}
			for (int j = intervalsSaved.size(); j > intervalsSaved.size()-upperBound; --j)
			{
				int temp;
				temp = static_cast<int>(intervalsSaved[j]/m_definitions->m_binSize);
				while (temp >= m_histogram_0.size())
				{
					m_histogram_0.push_back(0);
				}
				while (temp >= m_histogram_1t.size())
				{
					m_histogram_1t.push_back(0);
				}
				while (temp >= m_histogram_2t.size())
				{
					m_histogram_2t.push_back(0);
				}
				m_histogram_2t[temp]+=1.0;
			}
	//SZUKAMY MAXYMALNEJ WARTOŒCI
			double maxValue = 0;
			for (int j = 0; j < m_histogram_0.size(); ++j)
			{
				if( m_histogram_0[j] >= maxValue)
					maxValue = m_histogram_0[j];
			}
			//NORMALIZACJA
			for (int j = 0; j < m_histogram_0.size(); ++j)
			{
				m_histogram_0[j] /= maxValue;
			}
			maxValue = 0;
			for (int j = 0; j < m_histogram_1t.size(); ++j)
			{
				if( m_histogram_1t[j] >= maxValue)
					maxValue = m_histogram_1t[j];
			}
			//NORMALIZACJA
			for (int j = 0; j < m_histogram_1t.size(); ++j)
			{
				m_histogram_1t[j] /= maxValue;
			}
			maxValue = 0;
			for (int j = 0; j < m_histogram_2t.size(); ++j)
			{
				if( m_histogram_2t[j] >= maxValue)
					maxValue = m_histogram_2t[j];
			}
			//NORMALIZACJA
			for (int j = 0; j < m_histogram_2t.size(); ++j)
			{
				m_histogram_2t[j] /= maxValue;
			}

	//WYLICZANIE patEn
			//if(m_histogram.size()< 2*m_tau)
			//{
			//	  QMessageBox::information( this, "VarEn",
			//	 "Unable to calculate PatEn\n"
			//	 "Input file corrupted" );
			//	  return;
			//}
			double patEnSample = 0;
			double pi = 0.0;
			for (int j = 0; j < m_histogram_0.size(); ++j)
			{
				pi = m_histogram_0[j]*m_histogram_1t[j]*m_histogram_2t[j];
				if(pi != 0.0)
					patEnSample+= - pi* log(pi);
			}
			m_patEn = patEnSample;
			emit nextPatEn(m_patEn);
	}
}



void RRcalculator::calculateVarEn(double rr)
{

	double currentWindow;


	if( intervalsSaved.size() > m_definitions->m_windowSize)
	{
		m_histogram.clear();
		int upperBound = m_definitions->m_windowSize;
		
		currentWindow = upperBound;
	//ROBIMY HISTOGRAM
		for (int j = intervalsSaved.size(); j > intervalsSaved.size()-upperBound; j--)
		{
			int temp;
			temp = static_cast<int>(intervalsSaved[j]/m_definitions->m_binSize);
			while (temp >= m_histogram.size())
			{
				m_histogram.push_back(0);
			}
			m_histogram[temp]+=1.0;
		}
//SZUKAMY MAXYMALNEJ WARTOŒCI
		double maxValue = 0;
		for (int j = 0; j < m_histogram.size(); ++j)
		{
			if( m_histogram[j] >= maxValue)
				maxValue = m_histogram[j];
		}
//NORMALIZACJA
		for (int j = 0; j < m_histogram.size(); ++j)
		{
			m_histogram[j] /= maxValue;
		}
//WYLI CZANIE patEn
		double patEnSample = 0;
		double pi = 0.0;
		for (int j = 0; j < m_histogram.size()-2*m_definitions->m_tau; ++j)
		{
			pi = m_histogram[j]*m_histogram[j + m_definitions->m_tau]*m_histogram[j + 2*m_definitions->m_tau];
			if(pi != 0.0)
				patEnSample+= - pi* log(pi);
		}
		m_varEn = patEnSample;
			emit nextVarEn(m_varEn);

	}
}