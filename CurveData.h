#pragma once
//#include <qwt_data.h>
#include<vector>
using namespace std;
class CurveData
{
    // A container class for growing data
public:

    CurveData();

    void append(double *x, double *y, int count);

    int count() const;
    int size() const;
    const double *x() const;
    const double *y() const;
	void clear();

private:
    int d_count;
    vector<double> d_x;
    vector<double> d_y;
};
