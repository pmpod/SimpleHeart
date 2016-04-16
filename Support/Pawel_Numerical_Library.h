//---------------------------------------------------------------------------

//calculate hilbert transform
//save output from the center area
//adjust values to physiological	20 min
//prepare matlab scripts


#include <limits>
#include <vector>
#include <complex>

#include <fstream>

using namespace std;

#ifndef Pawel_Numerical_LibraryH
#define Pawel_Numerical_LibraryH
//---------------------------------------------------------------------------

class Pawel_Numerical_Library
{
    public:

	Pawel_Numerical_Library();

    //------------------------------------------------------------
    //------------------------------------------------------------
    // STATISTICAL TESTS
    //------------------------------------------------------------
    //------------------------------------------------------------

    // Rayleigh test for circular uniformity
    void circ_rtest(vector<double>* Angle_Data, double* p_val,double* z_stat);
    double circ_r(vector<double>* Angle_Data);


    // varia stats
    void calculate_max_min_mean_vec_double_ranged(std::vector<double> *Signal,
                long Begin,long End,double* Min, double* Max, double* Mean);
    void stdev_cor_vec_double(std::vector <double> *vec, double* average, double* SDev);
    void stdev_cor_vec_long(std::vector <long> *vec, double* average, double* SDev);

	// MPD - multiple peak detection
	void calculate_MPD_peaks_positions(
	vector<double> *Data_Vec, long Start,long Stop, double Time_Step,
	bool Offset_to_Ref_Bar, long Ref_Bar_Position, long Roving_Bar_Position,
	long Window_Size,
	double PP_Threshold,
	long Peak_Definition_Range,
	vector <long> *Peaks_Positions_In_Signal,
	double* Peak_Interval_Mean,
	double* Peak_Interval_SD,
	double* Last_Peak_Position_Relative_To_Roving_Position);

	// MPD - multiple peak detection  - Hamburg version
	void calculate_MPD_peaks_positions_Hamburg(
	vector<double> *Data_Vec, long Start,long Stop, double Time_Step,
	bool Offset_to_Ref_Bar, long Ref_Bar_Position,
	double PP_Threshold,
	long Peak_Definition_Range,
	vector <long> *Peaks_Positions_In_Signal,
	double* Peak_Interval_Mean,
	double* Peak_Interval_SD);

	// Histogramming
	double get_percentile_from_histogram(vector<long>* Histo,double Percentile,
						double Start_Value, double Box_Size);

	// Filtering
	double get_median(vector<double> Data);
	int median_filter(vector<double> &Signal,long Window_Length);


	void calculate_max_min_mean_vec_double(std::vector<double> *Signal,
				double* Min, double* Max, double* Mean);

	  // Function: matrix
	  // Description: creation of 2 dim. table
	  // Input parameters:
	  // n : long - number of rows
	  // m : long - number of columns
	  // Returns:
	  //   X** - polonger to table of polongers to doubles ( that is to a matrix )
	  double** matrix_doubles(long n, long m);
	  long** matrix_longs(long n, long m);
	  void free_matrix_doubles(double** mat, long n, long m);
	  void free_matrix_longs(long** mat, long n, long m);

      void rescale_signal(vector <double>* Signal,double Min,double Max);


 	// Hilbert transfrom using convolution
	void convol(double *source, double *target,double *filt, int npt, int lfilt);
	vector <long> get_Hilbert_phase_using_convolution_long(vector<long> *Signal);
	vector <double> get_Hilbert_phase_using_convolution_double(vector<double> *Signal);

      // DYSSYNCHRONY FUNCTIONS
      vector <double> recompose_bipolar_signal(vector<double> *Signal,int Wavelet_Period,int Electrogram_Type);
	  double get_phase_synchronization(vector<double> *Signal_A,vector<double> *Signal_B,int n,int m,bool Take_80_Perc_Flag);
      vector <double> unwrap_phase(vector <double> *Phase_Vec);

      double get_MPC_between_electrograms(std::vector<double> *Electrogram_1,
                                          std::vector<double> *Electrogram_2,
                                          long Basic_Cycle_Length,int Electrogram_Type);

	  void get_rgb_colors_from_ranged_variable(double Min,double Max, double Value,
											   int* R, int* G,int* B);

	vector <double> Butt_band_40_250_3th_order(vector <double> *Signal, bool Inverse);
	vector <double> rectify_signal(vector <double> *Signal);
	vector <double> Butt_low_20_3th_order(vector <double> *Signal, bool Inverse);
	double x7[7],y7[7];
	double x4[7],y4[7];

	long calculate_max_min_mean_vec_long(vector<long> Signal,long Begin, long End,
		double* Min, double* Max, double* Mean, int* Peak_Direction);


};

#endif
