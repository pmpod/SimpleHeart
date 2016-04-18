//---------------------------------------------------------------------------
#pragma hdrstop

#include "Pawel_Numerical_Library.h"
#include <algorithm>    // std::sort
#include <math.h>    // std::sort
#include <boost/math/constants/constants.hpp>

#define M_PI 3.141592653

using namespace std;

//---------------------------------------------------------------------------
//#pragma package(smart_init)

Pawel_Numerical_Library::Pawel_Numerical_Library()
{

	for(int i=0;i<7;i++)
	{
		x7[i]=0;
		y7[i]=0;
	}
	for(int i=0;i<4;i++)
	{
		x4[i]=0;
		y4[i]=0;
	}

}

//------------------------------------------------------------------
void Pawel_Numerical_Library::circ_rtest(vector<double>* Angle_Data, double* p_val,double* z_stat)
{
//
//   Computes Rayleigh test for non-uniformity of circular data.
//   H0: the population is uniformly distributed around the circle
//   HA: the populatoin is not distributed uniformly around the circle
//   Assumption: the distribution has maximally one mode and the data is
//   sampled from a von Mises distribution!
//
//   Input:
//     Angle_Data	sample of angles in radians
//
//   Output:
//     pval  p-value of Rayleigh's test
//     z     value of the z-statistic
//
// PHB 7/6/2008
//
// References:
//   Statistical analysis of circular data, N. I. Fisher
//   Topics in circular statistics, S. R. Jammalamadaka et al.
//   Biostatistical Analysis, J. H. Zar
//
// Circular Statistics Toolbox for Matlab

// By Philipp Berens, 2009
// berens@tuebingen.mpg.de - www.kyb.mpg.de/~berens/circStat.html

    double r =  circ_r(Angle_Data);
    double n = Angle_Data[0].size();

    // compute Rayleigh's R (equ. 27.1)
    double R = n*r;

    // compute Rayleigh's z (equ. 27.2)
    z_stat[0] = R*R / n;

    // compute p value using approxation in Zar, p. 617
    p_val[0] = exp(sqrt(1+4*n+4*(n*n-R*R))-(1+2*n));
}

//---------------------------------------------------------------------------

double Pawel_Numerical_Library::circ_r(vector<double>* Angle_Data)
{
//   function r = circ_r(alpha)
//   Computes mean resultant vector length for circular data.
//
//   Input:
//     alpha	sample of angles in radians
//
//   Output:
//     r		mean resultant length
//
// PHB 7/6/2008
//
// References:
//   Statistical analysis of circular data, N.I. Fisher
//   Topics in circular statistics, S.R. Jammalamadaka et al.
//   Biostatistical Analysis, J. H. Zar
//
// Circular Statistics Toolbox for Matlab

// By Philipp Berens, 2009
// berens@tuebingen.mpg.de - www.kyb.mpg.de/~berens/circStat.html

    //  compute sum of cos and sin of angles
    // r = sum(exp(i*alpha));
    if( Angle_Data[0].size() > 0 )
    {

    double S_Sum=0,C_Sum=0;
    for(long i=0;i<Angle_Data[0].size();i++)
    {
        S_Sum += sin(Angle_Data[0][i]);
        C_Sum += cos(Angle_Data[0][i]);
    }

    // obtain length
    // r = abs(r)./sum(w);
    return sqrt( S_Sum*S_Sum + C_Sum*C_Sum )/(double)(Angle_Data[0].size());

    }
    else
    return 100000000000;
}

//--------------------------------------------------------------------------

void Pawel_Numerical_Library::calculate_max_min_mean_vec_double_ranged(
				std::vector<double> *Signal,
				long Begin,long End,double* Min, double* Max, double* Mean)
{
	Min[0] = numeric_limits<double>::max();
	Max[0] = numeric_limits<double>::min();
	Mean[0] = 0.0;
	double counter=0;

	for(long i=Begin; i<End; i++)
	if( i >= 0  && i < Signal[0].size() )
	{

	Mean[0] += Signal[0][i];

	if( Signal[0][i] > Max[0] )
		Max[0] = Signal[0][i];

	if( Signal[0][i] < Min[0] )
		Min[0] = Signal[0][i];

	counter++;
	}

	if( counter != 0 )
	Mean[0] /= counter;
	else
	Mean[0]=0;

}

//---------------------------------------------------------------------------

void Pawel_Numerical_Library::stdev_cor_vec_double(std::vector <double> *vec,
	double* average, double* SDev)
{
	double SD,av = 0.0;
	long i;

	// 1. claculate average
	av = 0.0;
	for( i = 0; i <(signed) vec[0].size(); i++ )
		av += vec[0][i];

	if(vec[0].size()!=0)
		av  /= vec[0].size();
	else
		av = 0.0;

	average[0] = av;

	// 2. calculate stdev

	SD = 0.0;
	for( i = 0; i <(signed) vec[0].size(); i++ )
		SD += pow( (double)(vec[0][i] - av), 2.0 );

	if (SD > 0 && vec[0].size() != 0 )
	SDev[0] = sqrt( SD / ( vec[0].size()-1 ) );
	else SDev[0] = 0;

}

//---------------------------------------------------------------------------

void Pawel_Numerical_Library::stdev_cor_vec_long(std::vector <long> *vec,
	double* average, double* SDev)
{
	double SD,av = 0.0;
	long i;

	// 1. claculate average
	av = 0.0;
	for( i = 0; i <(signed) vec[0].size(); i++ )
		av += vec[0][i];

	if(vec[0].size()!=0)
		av  /= vec[0].size();
	else
		av = 0.0;

	average[0] = av;

	// 2. calculate stdev

	SD = 0.0;
	for( i = 0; i <(signed) vec[0].size(); i++ )
		SD += pow( (double)(vec[0][i] - av), 2.0 );

	if (SD > 0 && vec[0].size() != 0 )
	SDev[0] = sqrt( SD / ( vec[0].size()-1 ) );
	else SDev[0] = 0;

}

//---------------------------------------------------------------------------

void Pawel_Numerical_Library::calculate_MPD_peaks_positions_Hamburg(
	vector<double> *Data_Vec, long Start,long Stop, double Time_Step,

	bool Offset_to_Ref_Bar, long Ref_Bar_Position,

	double PP_Threshold,
	long Peak_Definition_Range,

	vector <long> *Peaks_Positions_In_Signal,
	double* Peak_Interval_Mean,
	double* Peak_Interval_SD)
{
	// adapted from
	// EDWARD J. CIACCIO, et al

	// Adapted to identify regions with fractionated electrograms

//	double min=0,max=0,mean=0;
//	calculate_max_min_mean_vec_double_ranged(Data_Vec,Start,Stop,&min, &max, &mean);
//	double PP_Threshold1 = PP_Threshold*(max-min);

	int Found,max_ptr,counter,min_ptr;
	Peaks_Positions_In_Signal[0].clear();

	//-----------------------------------------------------------
	// calculate all peaks
	//-----------------------------------------------------------
	bool pass_positive,pass_negative;

	for(long i=Start;i<Stop;i++)
	{

	// check for positive peak:
	pass_positive=true;
	for(long kk=-Peak_Definition_Range/Time_Step;kk<=Peak_Definition_Range/Time_Step;kk++)
	if(i+kk>=0 && i+kk<Data_Vec[0].size())
	if( Data_Vec[0][i+kk] > Data_Vec[0][i])
	pass_positive=false;

	// check for negative peak:
	pass_negative=true;
	for(long kk=-Peak_Definition_Range/Time_Step;kk<=Peak_Definition_Range/Time_Step;kk++)
	if(i+kk>=0 && i+kk<Data_Vec[0].size())
	if( Data_Vec[0][i+kk] < Data_Vec[0][i])
	pass_negative=false;

	if( pass_negative || pass_positive)
	{
		Peaks_Positions_In_Signal[0].push_back(i);
		i+=Peak_Definition_Range/Time_Step;
	}

	}

	// remove peaks if Peak is not greater than threshold
	bool sth_done=true;
	while(sth_done)
	{

	sth_done=false;

	for(long i=0;i<(signed)Peaks_Positions_In_Signal[0].size();i++)
	if( Peaks_Positions_In_Signal[0].size() >= 1 )
	if( Peaks_Positions_In_Signal[0][i]>0 &&
		Peaks_Positions_In_Signal[0][i]<Data_Vec[0].size())
	if(fabs(Data_Vec[0][Peaks_Positions_In_Signal[0][i]]) < PP_Threshold )
	{
		Peaks_Positions_In_Signal[0].erase(Peaks_Positions_In_Signal[0].begin() + Peaks_Positions_In_Signal[0][i]);
	sth_done=true;
	}

	}

	stdev_cor_vec_long(Peaks_Positions_In_Signal,Peak_Interval_Mean, Peak_Interval_SD);

	// offset mean to Ref bar if requested
	if( Offset_to_Ref_Bar )
//	for(long i=0;i<(signed)Peaks_Positions_In_Signal[0].size();i++)
//	Peaks_Positions_In_Signal[0][i] -= Ref_Bar_Position;
	Peak_Interval_Mean[0] -= Ref_Bar_Position;

	Peak_Interval_Mean[0] *= Time_Step;
	Peak_Interval_SD[0] *= Time_Step;
}

//---------------------------------------------------------------------------

void Pawel_Numerical_Library::calculate_MPD_peaks_positions(
	vector<double> *Data_Vec, long Start,long Stop, double Time_Step,

	bool Offset_to_Ref_Bar, long Ref_Bar_Position, long Roving_Bar_Position,

	long Window_Size,
	double PP_Threshold,
	long Peak_Definition_Range,

    vector <long> *Peaks_Positions_In_Signal,
    double* Peak_Interval_Mean,
    double* Peak_Interval_SD,
    double* Last_Peak_Position_Relative_To_Roving_Position
    )
{
    // adapted from
	// EDWARD J. CIACCIO, et al
    // Method to Predict Isthmus Location in Ventricular Tachycardia
    // Caused by Reentry with a Double-Loop Pattern

    double min=0,max=0,mean=0;
    int Found,max_ptr,counter,min_ptr;

    Peaks_Positions_In_Signal[0].clear();

    calculate_max_min_mean_vec_double_ranged(Data_Vec,Start,Stop,&min, &max, &mean);

//    double PP_Threshold1 = PP_Threshold*(max-min);

	//-----------------------------------------------------------
	// calculate all peaks
	//-----------------------------------------------------------
	bool pass_positive,pass_negative;

	for(long i=Roving_Bar_Position-0.5*Window_Size/Time_Step;i<Roving_Bar_Position+
		0.5*Window_Size/Time_Step;i++)
	if(i>Start && i<Stop)
	{

	// check for positive peak:
	pass_positive=true;
	for(long kk=-Peak_Definition_Range/Time_Step;kk<=Peak_Definition_Range/Time_Step;kk++)
	if(kk!=0)
	if( Data_Vec[0][i+kk] > Data_Vec[0][i])
	pass_positive=false;

	// check for negative peak:
	pass_negative=true;
	for(long kk=-Peak_Definition_Range/Time_Step;kk<=Peak_Definition_Range/Time_Step;kk++)
	if(kk!=0)
	if( Data_Vec[0][i+kk] < Data_Vec[0][i])
	pass_negative=false;

	if( pass_negative || pass_positive)
	{
		Peaks_Positions_In_Signal[0].push_back(i);
		i+=Peak_Definition_Range/Time_Step;
	}

	}

	// remove peaks if Peak is not greater than threshold
	bool sth_done=true;
	while(sth_done)
	{

	sth_done=false;

	for(long i=0;i<(signed)Peaks_Positions_In_Signal[0].size();i++)
	if( Peaks_Positions_In_Signal[0].size() > 1 )
	if( Peaks_Positions_In_Signal[0][i]>0 &&
		Peaks_Positions_In_Signal[0][i]<Data_Vec[0].size())
	if(fabs(Data_Vec[0][Peaks_Positions_In_Signal[0][i]]) < PP_Threshold )
	{
		Peaks_Positions_In_Signal[0].erase(Peaks_Positions_In_Signal[0].begin() + Peaks_Positions_In_Signal[0][i]);
	sth_done=true;
	}

	}

	// offset to Ref bar if requested
	if( Offset_to_Ref_Bar )
	for(long i=0;i<(signed)Peaks_Positions_In_Signal[0].size();i++)
	Peaks_Positions_In_Signal[0][i] -= Ref_Bar_Position;

	stdev_cor_vec_long(Peaks_Positions_In_Signal,Peak_Interval_Mean, Peak_Interval_SD);

	Peak_Interval_Mean[0] *= Time_Step;
	Peak_Interval_SD[0] *= Time_Step;

	// position of last
	Last_Peak_Position_Relative_To_Roving_Position[0] =
		(Peaks_Positions_In_Signal[0][Peaks_Positions_In_Signal[0].size()-1] -
			Roving_Bar_Position)*Time_Step;

}

//---------------------------------------------------------------------------

double Pawel_Numerical_Library::get_percentile_from_histogram(vector<long>* Histo,
				 double Percentile, double Start_Value, double Box_Size)
{
    long Size = Histo[0].size();

    if( Size > 2 )
	{

    // create cumulated histo
    double *CHisto = new double[Size];

    for(long i=0;i<Size;i++)
        CHisto[i] = 0;
    CHisto[0] = Histo[0][0];
    for(long i=1;i<Size;i++)
        CHisto[i] = CHisto[i-1] + Histo[0][i];

    // normalize
    if(CHisto[Size-1]!= 0)
    for(long i=0;i<Size;i++)
        CHisto[i] *= 100.0/CHisto[Size-1];

	double Ratio;
    int Box = 0;
    if( CHisto[Box]!=0 )
    if( CHisto[0] >= Percentile)
	{
        Box = 0;
        Ratio = (Percentile)/(CHisto[Box]);
    }
    else
    {
        while( CHisto[Box] < Percentile )
            Box++;

		// find ratio
		if( CHisto[Box]-CHisto[Box-1] != 0 )
		if( Box-1>=0)
		Ratio = (Percentile -CHisto[Box-1] )/(CHisto[Box]-CHisto[Box-1]);
		else
		Ratio = -1000;

	}

	return Start_Value + (Box+Ratio)*Box_Size;

	}
	else
	return -1;
}

//---------------------------------------------------------------------------

double Pawel_Numerical_Library::get_median(vector<double> Data)
{
  double median;
  size_t size = Data.size();

  sort(Data.begin(), Data.end());

  if (size  % 2 == 0)
	  median = (Data[size / 2 - 1] + Data[size / 2]) / 2;
  else
	  median = Data[size / 2];

  return median;
}

//---------------------------------------------------------------------------

int Pawel_Numerical_Library::median_filter(vector<double> &Signal,long Window_Length)
{
	vector<double> Filtered_Signal;
	long N=Signal.size();
	double v=0;
	Filtered_Signal.assign(N,v);
	double tmp;

	vector <double> W;

	for(long t=0;t<Window_Length;t++)
	W.push_back(Signal[t]);

	for(long t=Window_Length/2;t<N-Window_Length/2;t++)
	{

	tmp = get_median(W);
	Filtered_Signal[t] = tmp;

	W.push_back(Signal[t+Window_Length/2]);
	W.erase(W.begin() + W[0]);

	}

	for(long t=Window_Length/2;t<N-Window_Length/2;t++)
	Signal[t] = Filtered_Signal[t];
	return 1;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

	  // Function: matrix
	  // Description: creation of 2 dim. table of doubles
	  // Input parameters:
	  // n : long - number of rows
      // m : long - number of columns
      // Returns:
	  //   double** - polonger to table of polongers to doubles ( that is to a matrix )

double** Pawel_Numerical_Library::matrix_doubles(long n, long m )
{
	long i;
	double** mat;

	// Allocate polongers to rows
	mat = (double**) malloc((unsigned) (n)*sizeof(double*));

	if (!mat)
		exit( 0 );

	// Allocate rows and set polongers to them
	for (i = 0; i < n; i++)
	{
	mat[i] = (double*) malloc((unsigned) (m)*sizeof(double));

	if (!mat[i])
		exit( 0 );

	}

	for(long i=0;i<n;i++)
	for(long j=0;j<m;j++)
	mat[i][j]=0;

	return mat;
}

//----------------------------------------------------------------------------------
	  // Function: matrix
	  // Description: creation of 2 dim. table of doubles
	  // Input parameters:
	  // n : long - number of rows
	  // m : long - number of columns
	  // Returns:
	  //   double** - polonger to table of polongers to doubles ( that is to a matrix )

long** Pawel_Numerical_Library::matrix_longs(long n, long m )
{
	long i;
	long** mat;

	// Allocate polongers to rows
	mat = (long**) malloc((unsigned) (n)*sizeof(long*));
	if (!mat)
		exit( 0 );

	// Allocate rows and set polongers to them
	for (i = 0; i < n; i++)
	{
	mat[i] = (long*) malloc((unsigned) (m)*sizeof(long));
	if (!mat[i])
		exit( 0 );

	}

	for(long i=0;i<n;i++)
	for(long j=0;j<m;j++)
	mat[i][j]=0;

	return mat;
}

//----------------------------------------------------------------------------------

	  // Function: free_matrix
	  // Description: destruction of structure allocated by matrix function
	  // Input parameters:
	  // mat : double** - polonger to matrix
	  // n : long - number of rows
	  // m : long - number of columns

void Pawel_Numerical_Library::free_matrix_doubles(double** mat, long n, long m)
{
   for (long i=n-1;i>=0;i--)
   free ( (double*) ( mat[i] ) );

   free (( double*) ( mat ) );
}

//----------------------------------------------------------------------------------

	  // Function: free_matrix
	  // Description: destruction of structure allocated by matrix function
	  // Input parameters:
	  // mat : double** - polonger to matrix
	  // n : long - number of rows
	  // m : long - number of columns

void Pawel_Numerical_Library::free_matrix_longs(long** mat, long n, long m)
{
   for (long i=n-1;i>=0;i--)
   free ( (long*) ( mat[i] ) );

   free (( long*) ( mat ) );
}

//----------------------------------------------------------------------------------

double Pawel_Numerical_Library::get_MPC_between_electrograms(
                    std::vector<double> *Electrogram_1,
                    std::vector<double> *Electrogram_2,
                    long Basic_Cycle_Length,
                    int Electrogram_Type)
{
    vector <double> S1;
    vector <double> S2;

    // 1. Get recomposed signals
	S1 = recompose_bipolar_signal( Electrogram_1,Basic_Cycle_Length,Electrogram_Type);
	S2 = recompose_bipolar_signal( Electrogram_2,Basic_Cycle_Length,Electrogram_Type);

    // 2. Get Hilbert phase
	S1 = get_Hilbert_phase_using_convolution_double(&S1);
	S2 = get_Hilbert_phase_using_convolution_double(&S2);

    // 3. Get coherence
    double Coh = get_phase_synchronization(&S1,&S2,1,1,true);

    S1.clear();
    S2.clear();
    
    return Coh;
}

//-----------------------------------------------------------------------------------

vector <double> Pawel_Numerical_Library::recompose_bipolar_signal(
	vector<double> *Signal,int Wavelet_Period,int Trigger_Type)
{
	vector <double> Recomposed_Signal;
	double v=0;
	Recomposed_Signal.assign(Signal[0].size(),v);
	for(long t=0;t<Recomposed_Signal.size();t++)
	Recomposed_Signal[t]=0;
	int T2 = (double)Wavelet_Period/2.;

	//--------------------------------------
	// 1. Generate sinusoidal wavelet
	//--------------------------------------
	double* SWavelet;
	SWavelet = new double[Wavelet_Period];
	for(long t=0;t<Wavelet_Period;t++)
	SWavelet[t] = sin(2*3.1415*(double)t/(double)Wavelet_Period);

	//--------------------------------------
	// 2. Recompose signal
	//--------------------------------------
	double dv;

	if( Trigger_Type == 0 ) // unipolar, down slope triggered
	for(long t=0;t<Signal[0].size()-1;t++)
	{
		for(long tp=-Wavelet_Period/2;tp<Wavelet_Period/2;tp++)
		if(t+tp>0 && t+tp < Signal[0].size() )
		{
		dv = Signal[0][t+1]-Signal[0][t];
		if( dv < 0 )
		Recomposed_Signal[t+tp] += -dv*
			SWavelet[tp+Wavelet_Period/2];
		}
	}

	if( Trigger_Type == 1 ) // bipolar, apmlitude triggered
	for(long t=0;t<Signal[0].size();t++)
	{
		for(long tp=-Wavelet_Period/2;tp<Wavelet_Period/2;tp++)
		if(t+tp>0 && t+tp < Signal[0].size() )
		Recomposed_Signal[t+tp] += fabs(Signal[0][t])*
			SWavelet[tp+Wavelet_Period/2];
	}

	if( Trigger_Type == 2 ) // AP, up slope
	for(long t=0;t<Signal[0].size()-1;t++)
	{
		for(long tp=-T2;tp<T2;tp++)
		if(t+tp>0 && t+tp < Signal[0].size() )
		{
		dv = Signal[0][t+1]-Signal[0][t];
		if( dv > 0 )
		Recomposed_Signal[t+tp] += dv*SWavelet[tp+T2];
		}
	}

	delete [] SWavelet;
	return Recomposed_Signal;
}

//----------------------------------------------------------------------------------

double Pawel_Numerical_Library::get_phase_synchronization(
	vector<double> *Signal_A,vector<double> *Signal_B,int n,int m,
        bool Take_80_Perc_Flag)
{
    double Sins=0,Coss=0;
    double Coherence;

    if( Signal_A[0].size() > 0 && Signal_B[0].size() > 0 &&
        Signal_A[0].size() == Signal_B[0].size() )
    {

    //--------------------------------------------------------------------
    // 1. Unwrap signals
    //--------------------------------------------------------------------
    vector <double> S1,S2;
    S1 = unwrap_phase(Signal_A);
    S2 = unwrap_phase(Signal_B);

    //--------------------------------------------------------------------
    // 2. Compute synchronization
    //--------------------------------------------------------------------

    Take_80_Perc_Flag = false; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if( !Take_80_Perc_Flag )
    {
        for(long t=0;t<S1.size();t++)
        {
			Sins += sin((double)n*S1[t]-(double)m*S2[t]);
			Coss += cos((double)n*S1[t]-(double)m*S2[t]);
		}
        Coherence = sqrt( Sins*Sins + Coss*Coss )/(double)(S1.size());
	}

	if( Take_80_Perc_Flag )
	{
		for(long t=0.1*S1.size();t<=0.9*S1.size();t++)
		{
			Sins += sin((double)n*S1[t]-(double)m*S2[t]);
			Coss += cos((double)n*S1[t]-(double)m*S2[t]);
		}
		Coherence = sqrt( Sins*Sins + Coss*Coss )/(double)(0.8*S1.size());
	}

    return Coherence;

	}
	else
	return -1000000;
}


//------------------------------------------------------------------------------

vector <double> Pawel_Numerical_Library::unwrap_phase(vector <double> *Phase_Vec)
{
	vector <double> Unwrapped_Phase;
	Unwrapped_Phase = Phase_Vec[0];
	long N = Phase_Vec[0].size();
	double tmp=0;

	vector <double> dp;
	vector <double> dps;
	vector <double> dp_corr;
	vector <double> cumsum;

	dp.assign(N,tmp);
	dps = dp;
	dp_corr = dp;
	cumsum = dp;

	double cutoff = M_PI;               /* default value in matlab */

   // incremental phase variation
   // MATLAB: dp = diff(p, 1, 1);
	for (long j = 0; j < N-1; j++)
	  dp[j] = Phase_Vec[0][j+1] - Phase_Vec[0][j];

   // equivalent phase variation in [-pi, pi]
   // MATLAB: dps = mod(dp+dp,2*pi) - pi;
	for (long j = 0; j < N-1; j++)
	  dps[j] = (dp[j]+M_PI) - floor((dp[j]+M_PI) / (2*M_PI))*(2*M_PI) - M_PI;

   // preserve variation sign for +pi vs. -pi
   // MATLAB: dps(dps==pi & dp>0,:) = pi;
	for (long j = 0; j < N-1; j++)
	  if ((dps[j] == -M_PI) && (dp[j] > 0))
		dps[j] = M_PI;

   // incremental phase correction
   // MATLAB: dp_corr = dps - dp;
	for (long j = 0; j < N-1; j++)
	  dp_corr[j] = dps[j] - dp[j];

   // Ignore correction when incremental variation is smaller than cutoff
   // MATLAB: dp_corr(abs(dp)<cutoff,:) = 0;
	for (long j = 0; j < N-1; j++)
	  if (fabs(dp[j]) < cutoff)
		dp_corr[j] = 0;

   // Find cumulative sum of deltas
   // MATLAB: cumsum = cumsum(dp_corr, 1);
	cumsum[0] = dp_corr[0];
	for (long j = 1; j < N-1; j++)
	  cumsum[j] = cumsum[j-1] + dp_corr[j];

   // Integrate corrections and add to P to produce smoothed phase values
   // MATLAB: p(2:m,:) = p(2:m,:) + cumsum(dp_corr,1);
	for (long j = 1; j < N; j++)
	  Unwrapped_Phase[j] += cumsum[j-1];

	return Unwrapped_Phase;
}

//---------------------------------------------------------------------------

void Pawel_Numerical_Library::convol(double *source, double *target,
									 double *filt, int npt, int lfilt)
{
	int i, l;
	double yt;

	for (l=1; l<=npt-lfilt+1; l++)
	{
		yt = 0.0;
		for (i=1; i<=lfilt; i++)
			yt = yt+source[l+i-1]*filt[lfilt+1-i];
		target[l] = yt;
	}

	/* shifting lfilt/1+1/2 points */
	for (i=1; i<=npt-lfilt; i++)
	{
		target[i] = 0.5*(target[i]+target[i+1]);
	}

	for (i=npt-lfilt; i>=1; i--)
		target[i+lfilt/2]=target[i];

	/* writing zeros */
	for (i=1; i<=lfilt/2; i++)
	{
		target[i] = 0.0;
		target[npt+1-i] = 0.0;
	}
}


//----------------------------------------------------------------------------------


vector <long> Pawel_Numerical_Library::get_Hilbert_phase_using_convolution_long(
	vector<long> *Signal)
{
	int lfilt = 256; // length of the hilbert envelope
	double* hilb;
	hilb = new double[lfilt];
	vector <long> Result;

	double* x;
	double* xh;
	double* ampl;
	double* phase;

	double pi, pi2, xt, xht;
	pi = M_PI;
	pi2 = 2*pi;

	long N; // length of signal
	N = Signal[0].size();

	if( N > 3 )
	{

	x = new double[N];
	xh = new double[N];
	ampl = new double[N];
	phase = new double[N];

	for(long i=0;i<N;i++)
	{
		x[i] = Signal[0][i];
		xh[i] = 0;
		ampl[i] = 0;
		phase[i] = 0;
	}

	// generation of the envelope
	for (long i=1; i<=lfilt; i++)
		hilb[i]=1/((i-lfilt/2)-0.5)/pi;

	// Hilbert transform
	convol(x, xh, hilb, N-1, lfilt);

	// Ampl and phase
	for (long i=lfilt/2+1; i<=N-1-lfilt/2; i++)
	{
		xt = x[i];
		xht = xh[i];
		ampl[i] = sqrt(xt*xt+xht*xht);
		if( xht!=0 && xt!=0 )
		phase[i] = 100*atan2(xht ,xt);
		else
		phase[i] = 0;
	}

	for(long i=0;i<N;i++)
	{
		Result.push_back(phase[i]);
	}

	delete [] x;
	delete [] xh;
	delete [] ampl;
	delete [] phase;
	delete [] hilb;

	}

	return Result;
}

//---------------------------------------------------------------------------

vector <double> Pawel_Numerical_Library::get_Hilbert_phase_using_convolution_double(
	vector<double> *Signal)
{
	int lfilt = 256; // length of the hilbert envelope
	double* hilb;
	hilb = new double[lfilt];
	vector <double> Result;
	double* x;
	double* xh;
	double* ampl;
	double* phase;
	double pi, pi2, xt, xht;
	pi = M_PI;
	pi2 = 2*pi;

	long N; // length of signal
	N = Signal[0].size();

	if( N > lfilt )
	{

	x = new double[N];
	xh = new double[N];
	ampl = new double[N];
	phase = new double[N];

	for(long i=0;i<N;i++)
	{
		x[i] = Signal[0][i];
		xh[i] = 0;
		ampl[i] = 0;
		phase[i] = 0;
	}

//	ofstream df("sig.csv");
//	for(long i=0;i<N;i++)
//	df << Signal[0][i] << endl;
//	df.close();

	// generation of the envelope
	for (long i=0; i<lfilt; i++)
		hilb[i]=1/((i-lfilt/2)-0.5)/pi;

	// Hilbert transform
	convol(x, xh, hilb, N-1, lfilt);

	// Ampl and phase
	for (long i=lfilt/2+1; i<=N-1-lfilt/2; i++)
	{
		xt = x[i];
		xht = xh[i];
		ampl[i] = sqrt(xt*xt+xht*xht);
		if( xht!=0 && xt!=0 )
		phase[i] = atan2(xt,-xht); // modified so phase inv is at deflection
		else
		phase[i] = 0;
	}

	for(long i=0;i<N;i++)
		Result.push_back(phase[i]);

    // set range to -pi to +pi
    rescale_signal(&Result,-M_PI,+M_PI);

	delete [] x;
	delete [] xh;
	delete [] ampl;
	delete [] phase;
	delete [] hilb;

	}

	return Result;
}

//----------------------------------------------------------------------------------

void Pawel_Numerical_Library::rescale_signal(vector <double>* Signal,
	double Min,double Max)
{
	// find min max
	double Smin,Smax,Smean;
	calculate_max_min_mean_vec_double(Signal,&Smin, &Smax, &Smean);

	if( Smax!=Smin && Smin!=Smean && Smax!=Smean )
	{

	// transform signal
	double a = (Max-Min)/(Smax-Smin);
	double b = Max - a*Smax;

	for(long t=0;t<Signal[0].size();t++)
	Signal[0][t] = a*Signal[0][t]+b;

	}
}


//----------------------------------------------------------------------------------


void Pawel_Numerical_Library::calculate_max_min_mean_vec_double(
		std::vector<double> *Signal,double* Min, double* Max, double* Mean)
{
	long i;

	// 1. find max,min,mean from last DATA_LENGTH_FOR_PEAK_EXTRACTION
	// steps in roving and ref. cath
	Min[0] = std::numeric_limits< double >::max();
	Max[0] = std::numeric_limits< double >::min();
	Mean[0] = 0.0;

	for(i=0; i<Signal[0].size(); i++)
	{

	Mean[0] += Signal[0][i];

	if( Signal[0][i] > Max[0] )
		Max[0] = Signal[0][i];

	if( Signal[0][i] < Min[0] )
		Min[0] = Signal[0][i];
	}

	if( Signal[0].size() != 0 )
	Mean[0] /= (double)(Signal[0].size());
	else
	Mean[0]=0;
}

//----------------------------------------------------------------------------------

void Pawel_Numerical_Library::get_rgb_colors_from_ranged_variable(double Min,double Max, double Value,
											   int* R, int* G,int* B)
{
	double ratio = 2 * (Value-Min) / (Max - Min);
	B[0] = max(0., 255*(1 - ratio));
	R[0] = max(0., 255*(ratio - 1));
	G[0] = 255 - B[0] - R[0];
}

//----------------------------------------------------------------------------------

vector <double> Pawel_Numerical_Library::Butt_band_40_250_3th_order(vector <double> *Signal, bool Inverse)
{
	double gain  = 5.090882386e+01;
	vector <double> Result;
	vector <double> Rev_Result;

	for(int i=0;i<7;i++)
	{ x7[i]=0; y7[i]=0; }

	//--------------------------------------------
	if( ! Inverse )
	//--------------------------------------------
	{

	for(long k = 0; k < Signal[0].size(); k ++ )
	{
		x7[0] = x7[1];
		x7[1] = x7[2];
		x7[2] = x7[3];
		x7[3] = x7[4];
		x7[4] = x7[5];
		x7[5] = x7[6];
		x7[6] = Signal[0][k];// / gain;

		y7[0] = y7[1];
		y7[1] = y7[2];
		y7[2] = y7[3];
		y7[3] = y7[4];
		y7[4] = y7[5];
		y7[5] = y7[6];
		y7[6] =
	   ( -1/gain * x7[0])
	 + (  0/gain * x7[1])
	 + (  3/gain * x7[2])
	 + (  0/gain * x7[3])
	 + ( -3/gain * x7[4])
	 + (  0/gain * x7[5])
	 + (  1/gain * x7[6])

	 + ( -0.2662615629 * y7[0])
	 + (  1.8474876957 * y7[1])
	 + ( -5.4914172544 * y7[2])
	 + (  9.0007408198 * y7[3])
	 + ( -8.5801614461 * y7[4])
	 + (  4.4891238319 * y7[5]);

		Result.push_back(y7[6]);
	  }

	  return Result;

      } // no inverse

    //--------------------------------------------
	if( Inverse )
    //--------------------------------------------
    {

	for(long k = 0; k < Signal[0].size(); k ++ )
	{
		x7[0] = x7[1];
		x7[1] = x7[2];
		x7[2] = x7[3];
		x7[3] = x7[4];
		x7[4] = x7[5];
		x7[5] = x7[6];
		x7[6] = Signal[0][Signal[0].size()-1-k];// / gain;

		y7[0] = y7[1];
		y7[1] = y7[2];
		y7[2] = y7[3];
		y7[3] = y7[4];
		y7[4] = y7[5];
		y7[5] = y7[6];
		y7[6] =
	   ( -1/gain * x7[0])
	 + (  0/gain * x7[1])
	 + (  3/gain * x7[2])
	 + (  0/gain * x7[3])
	 + ( -3/gain * x7[4])
	 + (  0/gain * x7[5])
	 + (  1/gain * x7[6])

	 + ( -0.2662615629 * y7[0])
	 + (  1.8474876957 * y7[1])
	 + ( -5.4914172544 * y7[2])
	 + (  9.0007408198 * y7[3])
	 + ( -8.5801614461 * y7[4])
	 + (  4.4891238319 * y7[5]);

		Result.push_back(y7[6]);
	}

	// inverse result
	for(long k=0;k<Result.size();k++)
	Rev_Result.push_back(Result[Result.size()-1-k]);

	return Rev_Result;

	  } // inverse
}

//---------------------------------------------------------------------------
vector <double> Pawel_Numerical_Library::rectify_signal(vector <double> *Signal)
{
	vector <double> Result;

	for(long i=0;i<Signal[0].size();i++)
	Result.push_back(fabs(Signal[0][i]));

	return Result;
}

//---------------------------------------------------------------------------
vector <double> Pawel_Numerical_Library::Butt_low_20_3th_order(vector <double> *Signal, bool Inverse)
{
//	double gain  = 3.605241749e+04; // previous; 2048 sampling rate?
    double gain;
	vector <double> Result;
	vector <double> Rev_Result;
    x4[0]=0;x4[1]=0;x4[2]=0;x4[3]=0;

    //--------------------------------------------
    if( ! Inverse )
    //--------------------------------------------
    {

    gain = 4.553605266e+03;
    for(long k = 0; k < Signal[0].size(); k ++ )
    {
        x4[0] = x4[1];
        x4[1] = x4[2];
		x4[2] = x4[3];
        x4[3] = Signal[0][k] / gain;
        y4[0] = y4[1];
        y4[1] = y4[2];
        y4[2] = y4[3];
        y4[3] =   (x4[0] + x4[3]) + 3 * (x4[1] + x4[2])
                     + (  0.7776385602 * y4[0]) + ( -2.5282312191 * y4[1])
                     + (  2.7488358092 * y4[2]);
        Result.push_back(y4[3]);
	  }

/*
	for(long k = 0; k < Signal[0].size(); k ++ )
	{
		x4[0] = x4[1];
		x4[1] = x4[2];
		x4[2] = x4[3];
		x4[3] = Signal[0][k] / gain;

		y4[0] = y4[1];
		y4[1] = y4[2];
		y4[2] = y4[3];
		y4[3] =
	   (  1 * x4[0])
	 + (  3 * x4[1])
	 + (  3 * x4[2])
	 + (  1 * x4[3])

	 + ( 0.8837484610 * y4[0])
	 + ( -2.7604268125 * y4[1])
	 + ( 2.8764564523 * y4[2]);

		Result.push_back(y4[3]);
    }
*/


    return Result;

    }

    //--------------------------------------------
    if( Inverse )
    //--------------------------------------------
    {

    gain = 4.553605266e+03;
    for(long k = 0; k < Signal[0].size(); k ++ )
    {
        x4[0] = x4[1];
        x4[1] = x4[2];
        x4[2] = x4[3];
        x4[3] = Signal[0][Signal[0].size()-1-k] / gain;
        y4[0] = y4[1];
        y4[1] = y4[2];
        y4[2] = y4[3];
        y4[3] =   (x4[0] + x4[3]) + 3 * (x4[1] + x4[2])
                     + (  0.7776385602 * y4[0]) + ( -2.5282312191 * y4[1])
                     + (  2.7488358092 * y4[2]);
        Result.push_back(y4[3]);
      }

    /*
	for(long k = 0; k < Signal[0].size(); k ++ )
	{
		x4[0] = x4[1];
		x4[1] = x4[2];
		x4[2] = x4[3];
		x4[3] = Signal[0][Signal[0].size()-1-k] / gain;

		y4[0] = y4[1];
		y4[1] = y4[2];
		y4[2] = y4[3];
		y4[3] =
	   (  1 * x4[0])
	 + (  3 * x4[1])
	 + (  3 * x4[2])
	 + (  1 * x4[3])

	 + ( 0.8837484610 * y4[0])
	 + ( -2.7604268125 * y4[1])
	 + ( 2.8764564523 * y4[2]);

		Result.push_back(y4[3]);
    }
    */

    // inverse result
    for(long k=0;k<Result.size();k++)
    Rev_Result.push_back(Result[Result.size()-1-k]);

    return Rev_Result;
    }
}

//---------------------------------------------------------------------------

long Pawel_Numerical_Library::calculate_max_min_mean_vec_long(vector<long> Signal,long Begin, long End,
		double* Min, double* Max, double* Mean, int* Peak_Direction)
{
	long i;

	// 1. find max,min,mean from last DATA_LENGTH_FOR_PEAK_EXTRACTION
	// steps in roving and ref. cath
	Min[0] = 10000000000;
	Max[0] = -10000000000;
	Mean[0] = 0.0;

	if( Begin == End ) End++;
	if( Begin < 0 )
		Begin = 0;

	if( Begin < End )
	{

	if( Begin < 0 || End >= Signal.size() )
	int werwer=234;
	else
	for(i=Begin; i<End; i++)
	{

	Mean[0] += Signal[i];

	if( Signal[i] > Max[0] )
		Max[0] = Signal[i];

	if( Signal[i] < Min[0] )
		Min[0] = Signal[i];

	}

	Mean[0] /= (End-Begin);

	// set peak direction
	if( fabs( Max[0] - Mean[0] ) > fabs( Min[0] - Mean[0] ) )
		Peak_Direction[0] = 1;
	else
		Peak_Direction[0] = -1;

	return 1;

	}
	else
	{

	Min[0] = 0.0;
	Max[0] = 1.0;
	Mean[0] = 0.0;

	return -1;

	}
}


