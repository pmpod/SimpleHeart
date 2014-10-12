#include "CurveData.h"

//------------------------------------------------------------
CurveData::CurveData():
    d_count(0)
{
}
//------------------------------------------------------------
void CurveData::append(double *x, double *y, int count)
{
    int newSize = ( (d_count + count) / 1000 + 1 ) * 1000;
    if ( newSize > size() )
    {
        d_x.resize(newSize);
        d_y.resize(newSize);
    }

    for ( register int i = 0; i < count; i++ )
    {
        d_x[d_count + i] = x[i];
        d_y[d_count + i] = y[i];
    }
    d_count += count;
}
//------------------------------------------------------------
int CurveData::count() const
{
    return d_count;
}
//------------------------------------------------------------
int CurveData::size() const
{
    return d_x.size();
}
//------------------------------------------------------------
const double *CurveData::x() const
{
    return d_x.data();
}
//------------------------------------------------------------
const double *CurveData::y() const
{
    return d_y.data();
}
//------------------------------------------------------------
void CurveData::clear()
{
    d_x.clear( );
    d_y.clear();
}